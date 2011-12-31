#!/usr/bin/perl
#poll ps and convert changes into Gource format.

#usage:
#    gource-ps.pl [[USER@]SERVER] | gource --log-format custom -
#
# (requires Gource 0.27 or later as needs working STDIN support)

use strict;
use warnings;
use Getopt::Long qw(GetOptions);

#use Data::Dumper;

my %opt = ();

if(!GetOptions(\%opt, 'debug|d')) {
    die("usage: gource-ps.pl [[USER\@]SERVER] | gource --log-format custom - ...\n");
}

my $windows = $^O =~ /win32|msys/i;

if($windows) {
    require Win32::Process::Info;
    Win32::Process::Info->import();
}

my($server) = @ARGV;
my $user = $ENV{USER};

if($server && $server =~ /(.+)\@(.+)/) {
    $user = $1;
    $server = $2;
}

die('remote ps via ssh unimplemented on Win32') if $windows && $server;

$|=1;

my %process;

sub _proc_list {

    my $ps_command =  'ps axo pid,ppid,user,time,comm';

    my @pslist = $server ? `ssh $user\@$server "$ps_command"` : `$ps_command`;

    if($?) {
        die("ps command failed: $!\n");
    }

    shift @pslist;

    chomp(@pslist);

    my @stack;

    #build process tree
    foreach my $line (@pslist) {
        $line =~ s/^\s+//;

        my ($pid, $ppid, $username, $time, @command) = split(/\s+/, $line);

        my $command = join(' ', @command) || '';
        $command =~ s{^.+/}{}g;

        my $proc;

        if($proc = $process{$pid}) {
            $proc->{status} = ($proc->{'time'} ne $time) ? 'M' : '';
        } else {
            $proc = {
                pid      => $pid,
                ppid     => $ppid,
                username => $username,
                command  => $command,
                'time'   => $time,
                status   => 'A',
            };

            $process{$pid} = $proc;
        }

        #warn Dumper($process{$pid});
    }
}

sub _win32_proc_list {
    my $pi = Win32::Process::Info->new ();

    foreach my $winproc ($pi->GetProcInfo) {

#        use Data::Dumper;
#        print Dumper($proc);

        my $pid      = $winproc->{ProcessId};
        my $ppid     = $winproc->{ParentProcessId};
        my $command  = $winproc->{Description};
        my $username = $winproc->{Owner} || 'System';
        my $time     = $winproc->{UserModeTime};

        $username =~ s/^.+\\//;
        $command  =~ s/\.exe$//i;

        next unless $command && $pid && $username;

        my $proc;

        if($proc = $process{$pid}) {
            $proc->{status} = ($proc->{'time'} ne $time) ? 'M' : '';
        } else {
            $proc = {
                pid      => $pid,
                ppid     => $ppid,
                username => $username,
                command  => $command,
                'time'   => $time,
                status   => 'A',
            };

            $process{$pid} = $proc;
        }


    }
}

sub _make_command_path {
    my $proc = shift;

    my @path;

    my $node = $proc;

    while($node) {
        push @path, $node->{pid} . '.' . $node->{command};
        $node = $node->{ppid} ? $process{$node->{ppid}} : 0;
    }

    return join('/', reverse @path);
}

while(1) {

    my @proclist = $windows ? _win32_proc_list : _proc_list;

    my @filter_pids;

    #filter ps / gource processes and parent processes owned by this user
    foreach my $pid (sort {$a <=> $b} keys %process) {
        my $proc = $process{$pid};

        next unless $proc;

        # delete ps process and parents of
        if($proc->{command} eq 'ps' && $proc->{username} eq $user || $proc->{command} eq 'gource') {
            while($proc) {
                push @filter_pids, $proc->{pid};
                $proc = $proc->{ppid} ? $process{$proc->{ppid}} : undef;
            }
        }
    }

    delete $process{$_} for @filter_pids;

    my @expired_pids;

    my $current_time = time;

    foreach my $pid (sort {$a <=> $b} keys %process) {

        my $proc = $process{$pid};

        if($proc->{status}) {
            my $output_line = join('|', $current_time, $proc->{username}, $proc->{status}, _make_command_path($proc)). "\n";
            print $output_line;
            print STDERR $output_line if $opt{debug};
        }

        #delete if not seen next time
        if($proc->{status} eq 'D') {
            push @expired_pids,  $pid;
        } else {
            $proc->{status} = 'D';
        }
    }

    #remove expired pids
    delete $process{$_} for @expired_pids;

    sleep(1);
}

