#!/usr/bin/perl
# windows archive + installer generator

use strict;
use warnings;
use FindBin;

my $base_dir   = "$FindBin::Bin/../..";
my $dll_dir    = "$base_dir/dev/win32";
my $builds_dir = "$base_dir/dev/builds";

sub gource_version {
    my $version = `cat $base_dir/src/gource_settings.h | grep GOURCE_VERSION`;
    $version =~ /"([^"]+)"/ or die("could not determine version\n");
    $version = $1;
    return $version;
}

sub doit {
    my $cmd = shift;
    
    if(system($cmd) != 0) {
	die("command '$cmd' failed: $!");
    }
}

sub dosify {
    my($src, $dest) = @_;
    
    my $content = `cat $src`;
    $content =~ s/\r?\n/\r\n/g;
    
    open  OUTPUT, ">$dest" or die("$!");
    print OUTPUT $content;
    close OUTPUT;
}

chdir("$base_dir") or die("chdir to $base_dir failed");

my $nsis_script = q[
!include "MUI2.nsh"
!include "EnvVarUpdate.nsh"

Name "Gource GOURCE_VERSION"

OutFile    "GOURCE_INSTALLER"
InstallDir $PROGRAMFILES\Gource

RequestExecutionLevel admin

!define MUI_WELCOMEFINISHPAGE_BITMAP   "..\..\nsis\welcome.bmp"
!define MUI_UNWELCOMEFINISHPAGE_BITMAP "..\..\nsis\welcome.bmp"

!define MUI_COMPONENTSPAGE_NODESC

!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_COMPONENTS
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH

!insertmacro MUI_LANGUAGE "English"

Section "Gource" SecGource
  SectionIn RO

  GOURCE_INSTALL_LIST

  writeUninstaller $INSTDIR\uninstall.exe

  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Gource" "DisplayName"          "Gource"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Gource" "DisplayVersion"       "GOURCE_VERSION"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Gource" "UninstallString"      '"$INSTDIR\uninstall.exe"'
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Gource" "QuietUninstallString" '"$INSTDIR\uninstall.exe" /S'
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Gource" "NoModify" 1
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Gource" "NoRepair" 1

SectionEnd

Section "Add to PATH" SecAddtoPath
  ${EnvVarUpdate} $0 "PATH" "A" "HKLM" "$INSTDIR"
SectionEnd

Section "Uninstall"
  ${un.EnvVarUpdate} $0 "PATH" "R" "HKLM" "$INSTDIR"
  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Gource"

  GOURCE_UNINSTALL_LIST
  GOURCE_UNINSTALL_DIRS

  Delete $INSTDIR\uninstall.exe
  RMDir "$INSTDIR"
SectionEnd
];

my @gource_files = qw(
    gource.exe
    README
    data/beam.png
    data/file.png
    data/user.png
    data/bloom.tga
    data/bloom_alpha.tga
    data/gource.style
    data/fonts/FreeSans.ttf
    data/shaders/bloom.frag 
    data/shaders/bloom.vert
    data/shaders/shadow.frag
    data/shaders/shadow.vert
    data/shaders/text.frag
    data/shaders/text.vert
);

my @gource_txts = qw(
    README
    ChangeLog
    data/fonts/README
    README-SDL
    COPYING
    THANKS
);

my @gource_dlls = qw(
    SDL.dll
    SDL_image.dll
    pcre3.dll
    jpeg.dll
    libpng12-0.dll
    zlib1.dll
    glew32.dll
    freetype6.dll
    libboost_filesystem-mgw47-1_52.dll
    libboost_system-mgw47-1_52.dll
    libgcc_s_dw2-1.dll
    libstdc++-6.dll
);

my @gource_dirs = qw(
    data
    data/fonts
    data/shaders
);

my $tmp_dir = "$builds_dir/gource-build.$$";

doit("rm $tmp_dir") if -d $tmp_dir;
mkdir($tmp_dir);

# create directories
foreach my $dir (@gource_dirs) {
    mkdir("$tmp_dir/$dir");
}

my @gource_bundle;

# copy general files
foreach my $file (@gource_files) {
    doit("cp $file $tmp_dir/$file");
    push @gource_bundle, $file;
}

# copy dlls
foreach my $file (@gource_dlls) {
    doit("cp $dll_dir/$file $tmp_dir/$file");
    push @gource_bundle, $file;
}

# convert text files
foreach my $file (@gource_txts) {
    dosify("$file", "$tmp_dir/$file.txt");
    push @gource_bundle, "$file.txt";
}

my $version = gource_version();

my $installer_name = "gource-${version}-setup.exe";
my $archive_name   = "gource-${version}.win32.zip";

my $install_list = '';

foreach my $dir ('', @gource_dirs) {

    my @dir_files = map  { my $f = $_; $f =~ s{/}{\\}g; $f; }
                    grep { my $d = /^(.+)\// ? $1 : ''; $d eq $dir }
                    @gource_bundle;

    (my $output_dir = $dir) =~ s{/}{\\}g;

    $install_list .= "\n" . '  SetOutPath "$INSTDIR' . ( $dir ? "\\$output_dir" : "" ) . "\"\n\n";

    foreach my $file (@dir_files) {
        $install_list .= '  File '.$file."\n";
    }
}

my $uninstall_list = join("\n", map { my $f = $_; $f =~ s{/}{\\}g; '  Delete $INSTDIR\\'.$f } @gource_bundle);
my $uninstall_dirs = join("\n", map { my $d = $_; $d =~ s{/}{\\}g; '  RMDir  $INSTDIR\\'.$d } reverse @gource_dirs);

$nsis_script =~ s/GOURCE_VERSION/$version/g;
$nsis_script =~ s/GOURCE_INSTALLER/$installer_name/g;
$nsis_script =~ s/GOURCE_INSTALL_LIST/$install_list/;
$nsis_script =~ s/GOURCE_UNINSTALL_LIST/$uninstall_list/;
$nsis_script =~ s/GOURCE_UNINSTALL_DIRS/$uninstall_dirs/;
$nsis_script =~ s/\n/\r\n/g;

chdir($tmp_dir) or die("failed to change directory to '$tmp_dir'\n");

# remove existing copies of the version installer if they exist

doit("rm ../$installer_name") if -e "../$installer_name";
doit("rm ../$archive_name")   if -e "../$archive_name";

my $output_file = "gource.nsi";

open my $NSIS_HANDLE, ">$output_file" or die("failed to open $output_file: $!");
print $NSIS_HANDLE $nsis_script;
close $NSIS_HANDLE;

# generate installer

doit("makensis $output_file");

doit("rm $output_file");
doit("mv $installer_name ..");

# also create zip archive

doit("zip -r $archive_name *");
doit("mv $archive_name ..");

chdir("$tmp_dir/..");
doit("rm -rf $tmp_dir");
