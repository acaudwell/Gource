#!/usr/bin/perl
#poll ps and convert changes into Gource format.

#usage:
#    gource-ps.pl [[USER@]SERVER] | gource --log-format custom -
#
# (requires Gource 0.27 or later as needs working STDIN support)

use strict;
use warnings;

#use Data::Dumper;

my($server) = @ARGV;
my $user = $ENV{USER};

if($server && $server =~ /(.+)\@(.+)/) {
    $user = $1;
    $server = $2;
}

$|=1;

my %process;

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

    my $ps_command = 'ps axo pid,ppid,user,comm,time';

    my @pslist = $server ? `ssh $user\@$server "$ps_command"` : `$ps_command`;

    if($?) {
        die("ps command failed: $!\n");
    }

    shift @pslist;

    chomp(@pslist);

    my @stack;

    my $current_time = time;

    #build process tree
    foreach my $line (@pslist) {
        $line =~ s/^\s+//;

        my($pid, $ppid, $username, $command, $time) = split(/\s+/, $line);

        $command =~ s{/}{:}g;

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

    my @filter_pids;

    #filter ps / gource processes and parent processes owned by this user
    foreach my $pid (sort {$a <=> $b} keys %process) {
        my $proc = $process{$pid};

        next unless $proc;

        # delete ps process and parents of
        if($proc->{command} eq 'ps' & $proc->{username} eq $user || $proc->{command} eq 'gource') {
            while($proc) {
                push @filter_pids, $proc->{pid};
                $proc = $proc->{ppid} ? $process{$proc->{ppid}} : undef;
            }
        }
    }

    delete $process{$_} for @filter_pids;

    my @expired_pids;

    foreach my $pid (sort {$a <=> $b} keys %process) {

        my $proc = $process{$pid};

        if($proc->{status}) {
            print join('|', $current_time, $proc->{username}, $proc->{status}, _make_command_path($proc)), "\n"
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

