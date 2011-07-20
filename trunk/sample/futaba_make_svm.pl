#!/usr/local/bin/perl
use strict;
use warnings;
use Data::Dumper;

my $loop  = shift @ARGV;
my $alpha = shift @ARGV;

# read file, and set data
my @data;
my %class_list;
while (<>) {
  chomp;
  my @a = split(/\t/);
  if (@a != 2) { warn "#$. is invalid. ignored.\n"; next; }
  my $class_key = $a[0];
  $class_key =~ s/(:.+)//;
  $class_list{$class_key}++;

  my %v;
  my @b = split(/,/, $a[1]);
  foreach (@b) {
    my @c = split(/:/);
    if (@c != 2) { warn "#$. ($_) is invalid. ignored.\n"; next; }
    $v{$c[0]} += $c[1];
  }
  push(@data, [$class_key, \%v]);
}


# train weight by sgd-svm algorithm
my @class_keys = keys %class_list;
my %ws;
foreach my $c (@class_keys) {
  $ws{$c} = {};
}
while ($loop > 0) {
  foreach my $d (@data) {
    foreach my $c (@class_keys) {
      my $w = $ws{$c};
      my $t = -1;
      if ($d->[0] eq $c) { $t = 1; }
      train($w, $d->[1], $t, $alpha);
    }
  }
  $loop--;
}


# invert weight
my %ii;
foreach my $c (@class_keys) {
  my $w = $ws{$c};
  foreach my $f (keys %$w) {
    $ii{$f}{$c} += $w->{$f};
  }
}


# print weight
foreach my $f (keys %ii) {
  print "$f\t";
  my $sep = '';
  my $cr = $ii{$f};
  foreach my $c (keys %$cr) {
    my $score = int($cr->{$c} * 10000) / 10000;
    print "$sep$c:$score";
    $sep = ',';
  }
  print "\n";
}


# sub routines
sub predict {
  my $w = shift;
  my $x = shift;

  my $y = 0;
  foreach my $f (keys %$x) {
    if ($w->{$f}) { $y += ($w->{$f} * $x->{$f}); }
  }
  return $y;
}

sub train {
  my $w = shift;
  my $x = shift;
  my $t = shift;
  my $a = shift;

  my $y = predict($w, $x);
  if ($y * $t < 1) {
    foreach my $f (keys %$x) {
      if (not $w->{$f}) { $w->{$f} = 0.0; }
      $w->{$f} += (($t * $x->{$f}) - $a * $w->{$f});
    }
  }
}


