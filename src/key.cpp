#include "key.h"


// Text Key Entry
// a string for the file label and a colour

TextKeyEntry::TextKeyEntry(const FXFont& font, const std::string& label, const vec3& colour) {
    this->label    = label;
    this->colour = colour;
    this->pos_y  = -1.0f;

    this->font = font;
    this->font.dropShadow(false);

    shadow      = vec2(3.0, 3.0);

    width       = std::max(90, gGourceSettings.text_rectangle_width);
    height      = 18.0f;
    left_margin = 20.0f;
    brightness  = 1.0f;
    alpha       = 0.0f;

    move_elapsed = 1.0f;
    src_y        = -1.0f;
    dest_y       = -1.0f;
    elapsed_time = 1.0f;

    show = true;

    display_ext = label;

    bool truncated = false;

    while(font.getWidth(display_ext) > width - 15.0f) {
        display_ext.resize(display_ext.size()-1);
        truncated = true;
    }

    if(truncated) {
        display_ext += std::string("...");
    }

    value.clear();
}

const vec3& TextKeyEntry::getColour() const {
    return colour;
}

const std::string& TextKeyEntry::getLabel() const {
    return label;
}

void TextKeyEntry::setShow(bool show) {
    this->show = show;
}

bool TextKeyEntry::isFinished() const {
    return (getValue()<=0 && alpha <= 0.0f);
}

void TextKeyEntry::colourize() {
    colour = label.empty() ? vec3(1.0f, 1.0f, 1.0f) : colourHash(label);
}

void TextKeyEntry::inc(bool autoexpire) {
    value.push_back(autoexpire ? elapsed_time : std::numeric_limits<float>::max());
}

void TextKeyEntry::dec() {
    value.erase(value.begin());
}

int TextKeyEntry::getValue() const {
    return value.size();
}

void TextKeyEntry::setValue(unsigned count, float elapsed_time) {
    value.resize(count, elapsed_time);
}

void TextKeyEntry::setDestY(float dest_y) {
    if(dest_y == this->dest_y) return;

    this->dest_y = dest_y;

    src_y = pos_y;
    move_elapsed = 0.0f;
}


void TextKeyEntry::logic(float dt) {
    elapsed_time += dt;

    removeExpiredEntries();

    if(getValue() <= 0 || !show) {
        alpha = std::max(0.0f, alpha - dt);
    } else if(alpha < 1.0f) {
        alpha = std::min(1.0f, alpha + dt);
    }

    //move towards dest
    if(pos_y != dest_y) {
        //initialize pos from dest if new
        if(pos_y < 0.0f) pos_y = dest_y;
        else {
            move_elapsed += dt;

            if(move_elapsed >= 1.0f) pos_y = dest_y;
            else pos_y = src_y + (dest_y - src_y) * move_elapsed;
        }
    }

    pos = vec2(alpha * left_margin, pos_y);
}

void TextKeyEntry::draw() {
    if(isFinished()) return;
    //label.draw();

    glDisable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glColor4f(0.0f, 0.0f, 0.0f, alpha * 0.333f);

    glPushMatrix();
        glTranslatef(shadow.x, shadow.y, 0.0f);

        glBegin(GL_QUADS);
            glVertex2f(pos.x,       pos.y);
            glVertex2f(pos.x,       pos.y + height);
            glVertex2f(pos.x+width, pos.y + height);
            glVertex2f(pos.x+width, pos.y);
        glEnd();
    glPopMatrix();

    glBegin(GL_QUADS);
        glColor4f(colour.x * 0.5f, colour.y * 0.5f, colour.z * 0.5f, alpha);
        glVertex2f(pos.x,         pos.y);
        glVertex2f(pos.x,         pos.y + height);
        glColor4f(colour.x, colour.y, colour.z, alpha);
        glVertex2f(pos.x + width, pos.y + height);
        glVertex2f(pos.x + width, pos.y);
    glEnd();

    glEnable(GL_TEXTURE_2D);

    font.setColour(vec4(1.0f, 1.0f, 1.0f, alpha));

    font.dropShadow(false);
    font.draw((int)pos.x+2, (int)pos.y+3,  display_ext.c_str());

    font.dropShadow(true);
    font.print((int)pos.x+width+4, (int)pos.y+3, "%d", getValue());
}

void TextKeyEntry::removeExpiredEntries() {

    float expire_time = gGourceSettings.file_idle_time;

    while (expire_time > 0.0f && value.size() > 0) {
        float diff_time = elapsed_time - value[0];

        if(diff_time <= expire_time) {
            break;
        }

        value.erase(value.begin());
    }
}

// Key

//maintain a key of all the current file types, updated periodically.
//new entries slide in and out / fade in fade out

TextKey::TextKey() {

}

TextKey::TextKey(float update_interval) {
    this->update_interval = update_interval;
    interval_remaining = 1.0f;
    font = fontmanager.grab("FreeSans.ttf", 16);
    font.dropShadow(false);
    font.roundCoordinates(false);
    show = true;
}

TextKey::~TextKey() {
    active_keys.clear();

    for(std::map<std::string, TextKeyEntry*>::iterator it = keymap.begin(); it != keymap.end(); it++) {
        TextKeyEntry* entry = it->second;
        delete entry;
    }
    keymap.clear();

}

void TextKey::setShow(bool show) {
    this->show = show;

    for(std::vector<TextKeyEntry*>::iterator it = active_keys.begin(); it != active_keys.end(); it++) {

        TextKeyEntry* entry = *it;

        entry->setShow(show);
    }
    interval_remaining = 0.0f;
}

void TextKey::colourize() {
    for(std::vector<TextKeyEntry*>::iterator it = active_keys.begin(); it != active_keys.end(); it++) {
        TextKeyEntry* entry = *it;
        entry->colourize();
    }
}

void TextKey::clear() {

    for(std::vector<TextKeyEntry*>::iterator it = active_keys.begin(); it != active_keys.end(); it++) {
        TextKeyEntry* entry = *it;
        entry->setValue(0);
    }

    interval_remaining = 0.0f;
}

void TextKey::inc(RFile* file) {

    TextKeyEntry* entry = 0;

    std::map<std::string, TextKeyEntry*>::iterator result = keymap.find(file->ext);

    if(result != keymap.end()) {
        entry = result->second;
    } else {
        entry = new TextKeyEntry(font, file->ext, file->getFileColour());
        keymap[file->ext] = entry;
    }

    entry->inc();
}


//decrement value of extension. if drops to zero, mark it for removal
void TextKey::dec(RFile* file) {

    std::map<std::string, TextKeyEntry*>::iterator result = keymap.find(file->ext);

    if(result == keymap.end()) return;

    TextKeyEntry* entry = result->second;

    entry->dec();
}

void TextKey::inc(const std::string &label, bool expires) {

    TextKeyEntry* entry = 0;

    std::map<std::string, TextKeyEntry*>::iterator result = keymap.find(label);

    if(result != keymap.end()) {
        entry = result->second;
    } else {
        entry = new TextKeyEntry(font, label, colourHash(label));

        keymap[label] = entry;
    }

    entry->inc();
}


//decrement value of label. if drops to zero, mark it for removal
void TextKey::dec(const std::string &label) {

    std::map<std::string, TextKeyEntry*>::iterator result = keymap.find(label);

    if(result == keymap.end()) return;

    TextKeyEntry* entry = result->second;

    entry->dec();
}

bool file_key_entry_sort (const TextKeyEntry* a, const TextKeyEntry* b) {

    //sort by Value
    if(a->getValue() != b->getValue())
        return (a->getValue() > b->getValue());

    //then by name (tie breaker)
    return a->getLabel().compare(b->getLabel()) < 0;
}

void TextKey::logic(float dt) {

    interval_remaining -= dt;

    //recalculate active_keys
    if(interval_remaining <= 0.0f) {

        if(show) {
            active_keys.clear();
            std::vector<TextKeyEntry*> finished_keys;

            for(std::map<std::string, TextKeyEntry*>::iterator it = keymap.begin(); it != keymap.end(); it++) {
                TextKeyEntry* entry = it->second;

                if(!entry->isFinished()) {
                    active_keys.push_back(entry);
                } else {
                    finished_keys.push_back(entry);
                }
            }

            //sort
            std::sort(active_keys.begin(), active_keys.end(), file_key_entry_sort);

            //limit to entries we can put onto the screen
            int max_visible_entries = std::max(0, (int)((display.height - 150.0f) / 20.0f));

            if (active_keys.size() > max_visible_entries) {
                active_keys.resize(max_visible_entries);
            }

            //set position

            float key_y = 20.0f;

            for(std::vector<TextKeyEntry*>::iterator it = active_keys.begin(); it != active_keys.end(); it++) {
                TextKeyEntry* entry = *it;
                if(entry->getValue()>0) {
                    entry->setDestY(key_y);
                }
                key_y += 20.0f;
            }

            //remove and delete finished entries
            for(std::vector<TextKeyEntry*>::iterator it = finished_keys.begin(); it != finished_keys.end(); it++) {
                TextKeyEntry* entry = *it;
                keymap.erase(entry->getLabel());
                delete entry;
            }
        }

        interval_remaining = update_interval;
    }

    for(std::vector<TextKeyEntry*>::iterator it = active_keys.begin(); it != active_keys.end(); it++) {

        TextKeyEntry* entry = *it;

        entry->logic(dt);
    }
}

void TextKey::draw() {

    for(std::vector<TextKeyEntry*>::iterator it = active_keys.begin(); it != active_keys.end(); it++) {
        TextKeyEntry* entry = *it;

        entry->draw();
    }

}

AuthorKey::AuthorKey() : TextKey() {
    is_commit_mode = !strcmp("commit", gGourceSettings.author_key_mode.c_str());
}

AuthorKey::~AuthorKey() {

    clear();
}

AuthorKey::AuthorKey(float update_interval) : TextKey(update_interval) {

    is_commit_mode = !strcmp("commit", gGourceSettings.author_key_mode.c_str());
}

void AuthorKey::clear() {

    TextKey::clear();
}

void AuthorKey::incCommit(const std::string &author) {
    if (!is_commit_mode) return;

    TextKey::inc(author, false);
}

void AuthorKey::incFile(const std::string &author) {
    if (is_commit_mode) return;

    TextKey::inc(author, true);
}
