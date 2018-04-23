#!/usr/bin/perl
# windows archive + installer generator

use strict;
use warnings;
use FindBin;
use File::Copy;

my $base_dir     = "$FindBin::Bin/../..";
my $build_dir    = "$base_dir/../build-gource-Desktop_Qt_MinGW_64bit-Release/release";
my $binaries_dir = "$base_dir/dev/win64";

my $builds_output_dir = "$base_dir/dev/builds";

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

sub update_binaries {
    copy("$build_dir/gource.exe", "$binaries_dir/gource.exe") or die("failed to copy $build_dir/gource.exe: $!\n");

    chdir($binaries_dir) or die("failed to change directory to $binaries_dir\n");

    for my $existing_dll (glob("*.dll")) {
        unlink($existing_dll) or die("failed to remove existing dll $existing_dll: $!\n");
    }

    my @dlls = `cygcheck ./gource.exe | grep msys`;

    for my $dll (@dlls) {
        $dll =~ s/^\s+//g;
        $dll =~ s/[\r\n]//g;
        my($name) = $dll =~ m{\\([^\\]+\.dll)$};
        
        warn "adding $name\n";
            
        copy($dll, "$binaries_dir/$name") or die "failed to copy $name: $!\n"; 
    }
}

my $nsis_script = q[
!define MULTIUSER_MUI
!define MULTIUSER_EXECUTIONLEVEL Highest
!define MULTIUSER_INSTALLMODE_COMMANDLINE
!define MULTIUSER_USE_PROGRAMFILES64
!define MULTIUSER_INSTALLMODE_DEFAULT_REGISTRY_KEY "Software\Gource"
!define MULTIUSER_INSTALLMODE_INSTDIR_REGISTRY_KEY "Software\Gource"
!define MULTIUSER_INSTALLMODE_DEFAULT_REGISTRY_VALUENAME "Install_Mode"
!define MULTIUSER_INSTALLMODE_INSTDIR_REGISTRY_VALUENAME "Install_Dir"
!define MULTIUSER_INSTALLMODE_INSTDIR "Gource"

!include "x64.nsh"
!include "MultiUser.nsh"
!include "MUI2.nsh"
!include "LogicLib.nsh"
!include "SafeEnvVarUpdate.nsh"

Name "Gource GOURCE_VERSION"

OutFile "GOURCE_INSTALLER"

!define MUI_WELCOMEFINISHPAGE_BITMAP   "..\..\nsis\welcome.bmp"
!define MUI_UNWELCOMEFINISHPAGE_BITMAP "..\..\nsis\welcome.bmp"

!define MUI_COMPONENTSPAGE_NODESC

!insertmacro MULTIUSER_PAGE_INSTALLMODE
!insertmacro MUI_PAGE_WELCOME
!define MUI_PAGE_HEADER_TEXT "Legal Disclaimer"
!insertmacro MUI_PAGE_LICENSE "..\..\nsis\disclaimer.txt"
!insertmacro MUI_PAGE_COMPONENTS
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH

!insertmacro MUI_LANGUAGE "English"

Function .onInit
  ${IfNot} ${RunningX64}
    MessageBox MB_OK "This installer requires 64-bit Windows"
    Quit
  ${EndIf}
  !insertmacro MULTIUSER_INIT
  ReadRegStr $R0 SHCTX "Software\Microsoft\Windows\CurrentVersion\Uninstall\Gource" "UninstallString"
  StrCmp $R0 "" done
  MessageBox MB_OKCANCEL|MB_ICONEXCLAMATION \
  "Gource appears to already be installed. $\n$\nClick OK to remove the previous version and continue the installation." \
  IDOK uninst
  Abort
 
 uninst:
  ClearErrors
  ExecWait $R0

 done:
 
FunctionEnd

Function un.onInit
  !insertmacro MULTIUSER_UNINIT
FunctionEnd 

Section "Gource" SecGource
  SectionIn RO

  GOURCE_INSTALL_LIST

  writeUninstaller $INSTDIR\uninstall.exe

  WriteRegStr SHCTX "Software\Gource" ${MULTIUSER_INSTALLMODE_INSTDIR_REGISTRY_VALUENAME} "$INSTDIR"
  WriteRegStr SHCTX "Software\Gource" ${MULTIUSER_INSTALLMODE_DEFAULT_REGISTRY_VALUENAME} "$MultiUser.InstallMode"

  WriteRegStr   SHCTX "Software\Microsoft\Windows\CurrentVersion\Uninstall\Gource" "DisplayName"          "Gource"
  WriteRegStr   SHCTX "Software\Microsoft\Windows\CurrentVersion\Uninstall\Gource" "DisplayVersion"       "GOURCE_VERSION"
  WriteRegStr   SHCTX "Software\Microsoft\Windows\CurrentVersion\Uninstall\Gource" "UninstallString"      '"$INSTDIR\uninstall.exe"'
  WriteRegStr   SHCTX "Software\Microsoft\Windows\CurrentVersion\Uninstall\Gource" "QuietUninstallString" '"$INSTDIR\uninstall.exe" /S'
  WriteRegDWORD SHCTX "Software\Microsoft\Windows\CurrentVersion\Uninstall\Gource" "NoModify" 1
  WriteRegDWORD SHCTX "Software\Microsoft\Windows\CurrentVersion\Uninstall\Gource" "NoRepair" 1

SectionEnd

Section "Add to PATH" SecAddtoPath
 
  ${If} $MultiUser.InstallMode == "AllUsers"
    ${EnvVarUpdate} $0 "PATH" "A" "HKLM" "$INSTDIR\cmd"
  ${ElseIf} $MultiUser.InstallMode == "CurrentUser"
    ${EnvVarUpdate} $0 "PATH" "A" "HKCU" "$INSTDIR\cmd"
  ${EndIf}

SectionEnd

Section "Uninstall"

  ${If} $MultiUser.InstallMode == "AllUsers"
    ${un.EnvVarUpdate} $0 "PATH" "R" "HKLM" "$INSTDIR\cmd"
  ${ElseIf} $MultiUser.InstallMode == "CurrentUser"
    ${un.EnvVarUpdate} $0 "PATH" "R" "HKCU" "$INSTDIR\cmd"
  ${EndIf}

  DeleteRegKey SHCTX "Software\Microsoft\Windows\CurrentVersion\Uninstall\Gource"
  DeleteRegKey SHCTX "Software\Gource"

  GOURCE_UNINSTALL_LIST
  GOURCE_UNINSTALL_DIRS

  Delete $INSTDIR\uninstall.exe
  RMDir "$INSTDIR"
SectionEnd
];

my @gource_files = qw(
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
    cmd/gource.cmd
    cmd/gource
);

my @gource_txts = qw(
    README
    ChangeLog
    data/fonts/README
    COPYING
    THANKS
);

my @bin_files = qw(
    gource.exe
    SDL2.dll
    SDL2_image.dll
    glew32.dll
    libboost_filesystem-mt.dll
    libboost_system-mt.dll
    libbz2-1.dll
    libfreetype-6.dll
    libgcc_s_seh-1.dll
    libglib-2.0-0.dll
    libgraphite2.dll
    libharfbuzz-0.dll
    libiconv-2.dll
    libintl-8.dll
    libjpeg-8.dll
    liblzma-5.dll
    libpcre-1.dll
    libpng16-16.dll
    libstdc++-6.dll
    libtiff-5.dll
    libwebp-7.dll
    libwinpthread-1.dll
    zlib1.dll
);

my @gource_dirs = qw(
    data
    data/fonts
    data/shaders
    cmd
);

mkdir($binaries_dir)      unless -d $binaries_dir;
mkdir($builds_output_dir) unless -d $builds_output_dir;

my $tmp_dir = "$builds_output_dir/gource-build.$$";

doit("rm $tmp_dir") if -d $tmp_dir;
mkdir($tmp_dir);

# create directories
foreach my $dir (@gource_dirs) {
    mkdir("$tmp_dir/$dir");
}

my @gource_bundle;

update_binaries();

chdir("$base_dir") or die("chdir to $base_dir failed");

# copy binaries
foreach my $file (@bin_files) {
    doit("cp $binaries_dir/$file $tmp_dir/$file");
    push @gource_bundle, $file;
}

# copy general files
foreach my $file (@gource_files) {
    doit("cp $file $tmp_dir/$file");
    push @gource_bundle, $file;
}

# convert text files
foreach my $file (@gource_txts) {
    dosify("$file", "$tmp_dir/$file.txt");
    push @gource_bundle, "$file.txt";
}

my $version = gource_version();

my $installer_name = "gource-${version}.win64-setup.exe";
my $archive_name   = "gource-${version}.win64.zip";

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

unlink("../$installer_name") if -e "../$installer_name";
unlink("../$archive_name")   if -e "../$archive_name";

my $output_file = "gource.nsi";

open my $NSIS_HANDLE, ">$output_file" or die("failed to open $output_file: $!");
print $NSIS_HANDLE $nsis_script;
close $NSIS_HANDLE;

# generate installer

# assert we have the long string build of NSIS
doit("makensis -HDRINFO | grep -q NSIS_MAX_STRLEN=8192");

doit("makensis $output_file");

doit("rm $output_file");
doit("mv $installer_name ..");

# also create zip archive

doit("zip -r $archive_name *");
doit("mv $archive_name ..");

chdir("$tmp_dir/..");
doit("rm -rf $tmp_dir");
