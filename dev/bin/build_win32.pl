#!/usr/bin/perl

use strict;
use warnings;
use FindBin;

my $WINBUILD = "$FindBin::Bin/../builds/win32/";
my $VERSION  = '0.14';

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

chdir("$FindBin::Bin/../../");

doit("rm -rf $WINBUILD");
doit("mkdir -p $WINBUILD");
doit("mkdir -p $WINBUILD/data/");
doit("mkdir -p $WINBUILD/data/fonts/");

doit("cp gource.exe $WINBUILD");
doit("cp data/*.png $WINBUILD/data/");
doit("cp data/fonts/*.ttf $WINBUILD/data/fonts/");

dosify('README',    "$WINBUILD/README.txt");
dosify('data/fonts/README', "$WINBUILD/data/fonts/README.txt");
dosify('README-SDL',"$WINBUILD/README-SDL.txt");
dosify('COPYING',   "$WINBUILD/COPYING.txt");
dosify('THANKS',    "$WINBUILD/THANKS.txt");

doit("cp dev/win32/SDL.dll $WINBUILD");
doit("cp dev/win32/SDL_image.dll $WINBUILD");
doit("cp dev/win32/pcre.dll $WINBUILD");
doit("cp dev/win32/ftgl.dll $WINBUILD");
doit("cp dev/win32/jpeg.dll $WINBUILD");
doit("cp dev/win32/libpng12-0.dll $WINBUILD");
doit("cp dev/win32/zlib1.dll $WINBUILD");

chdir($WINBUILD);
doit("zip -r gource-$VERSION.win32.zip *");

doit("cp gource-$VERSION.win32.zip ..");
chdir("$WINBUILD/..");
doit("rm -rf $WINBUILD");

