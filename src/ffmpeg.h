/*
    Copyright (C) 2009 Johannes Schindelin (johannes.schindelin@gmx.de)

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation; either version
    3 of the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef FFMPEG_FRAME_EXPORTER_H
#define FFMPEG_FRAME_EXPORTER_H

#ifdef HAVE_FFMPEG
extern "C" {
#define INT64_C(c) c##ll
#include "libavutil/avutil.h"
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
}
#endif

#include "core/display.h"

class FrameExporter {
protected:
    SDL_Surface *surface;
    char *pixels;
    size_t rowstride;

public:
    FrameExporter();
    virtual ~FrameExporter() {}
    virtual void initialize();
    void dump();
    virtual void dumpImpl();
};

#ifdef HAVE_FFMPEG
class FFMPEGExporter : public FrameExporter {

public:
	FFMPEGExporter(std::string filename, int bitrate, int framerate);
	~FFMPEGExporter();
	void initialize();
	void dumpImpl();

protected:
	AVFrame *picture, *tmp_picture;
	uint8_t *video_outbuf;
	int frame_count, video_outbuf_size;
    int framerate;
    int bitrate;
	std::string filename;
	AVOutputFormat *fmt;
	AVFormatContext *oc;
	AVStream *video_st;
	double video_pts;
        struct SwsContext *img_convert_ctx;

	AVStream *add_video_stream(AVFormatContext *oc, enum CodecID codec_id,
			int width, int height);
	AVFrame *alloc_picture(enum PixelFormat pix_fmt, int width, int height);
	void open_video(AVFormatContext *oc, AVStream *st);
	void write_video_frame(AVFormatContext *oc, AVStream *st);
	void close_video(AVFormatContext *oc, AVStream *st);
};

extern void initializeStdoutExporter();
extern void initializeMovieExporter(std::string filename, int bitrate);
extern void dumpFrame();
extern void cleanupFrameExporter();

#endif

#endif

