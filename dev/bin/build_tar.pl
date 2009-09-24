#!/usr/bin/perl
#copy stuff we want to go into the gource source tar ball

use strict;
use warnings;


use FindBin;
use File::Path;
use File::Copy;
use Date::Format;
use Getopt::Long qw(GetOptions);
use Cwd;

my $version = '0.13';

my @exclusions = (
    qr{^/config.status$},
    qr{^/config.log$},
    qr{^/debian/},
    qr{^/gource$},
    qr{^/dev/},
    qr{^/logs/},
    qr{/\.},
    qr{^/README-SDL$},
    qr{Makefile$},
    qr{\.o$},
    qr{^/todo.txt$},
    qr{^/build-stamp$},
);

my @inclusions = (
    qr{^/gource\.win32\.cbp$},
    qr{^/THANKS$},
    qr{^/COPYING$},
    qr{^/INSTALL$},
    qr{^/README$},
    qr{/Makefile\.in$},
    qr{^/aclocal\.m4$},
    qr{^/m4/.+\.m4$},
    qr{^/configure(?:\.ac)?$},
    qr{^/src/.+\.(?:cpp|h)$},
    qr{^/data/.+\.(?:png|ttf|1)$},
    qr{^/data/fonts/README$},
    qr{^/config\.guess$},
    qr{^/config\.sub$},
    qr{^/install-sh$},
);

my $tmp_path = "/var/tmp/gource-$version";

system("rm -r $tmp_path") if -d $tmp_path;
mkpath($tmp_path) or die("failed to make temp folder $tmp_path");

chdir("$FindBin::Bin/../../");

my @files = `find .`;

foreach my $file (@files) {
    $file =~ s/[\r\n]+//;
    $file =~ s/^\.//;

    (my $relfile = $file) =~ s{^/}{};
    (my $dir = $file)    =~ s{[^/]+$}{/};

    next if $file =~ /^\s*$/;
    next if -d $relfile;

    next if grep { $file =~ $_ } @exclusions;

    unless(grep { $file =~ $_ } @inclusions) {
        warn "nothing known about $file\n";
        next;
    }

    mkpath("$tmp_path/$dir");
    if(system("cp", "$relfile", "$tmp_path/$relfile") != 0) {
        die("error copying $file to $tmp_path/$relfile: $? $!\n");
    }
}

my $current_dir = cwd;
chdir("/var/tmp/");

my $archive = "gource-$version.tar.gz";

if(system("tar -czf $archive gource-$version") !=0) {
    die("failed to make archive $archive");
}



unlink("$FindBin::Bin/../builds/$archive");

my $builds_dir = "$FindBin::Bin/../builds/";

system('mkdir', '-p', $builds_dir);

unlink("$builds_dir/$archive") if -e "$builds_dir/$archive";

move("$archive", $builds_dir);
system("rm -r $tmp_path");

print "Built $archive\n";
