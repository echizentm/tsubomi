#!/usr/bin/env perl
use strict;
use warnings;
use Data::Dumper;

if (@ARGV < 2) {
  die "[USAGE] futaba_make_svm.pl loop alpha < input.txt > output.svm\n".
      "  loop : number of iteraitons.\n".
      "  alpha: rate of regularization. (loss() + alpha * (|w|^2) / 2)\n";
}

my $loop  = shift @ARGV;
my $alpha = shift @ARGV;

# read file, and set data
warn "read file, and set data.\n";
my @data;
my %class_list;
while (<>) {
  warn "#$.\n" unless ($. % 10000);
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
warn "train weight by sgd-svm algorithm.\n";
my @class_keys = keys %class_list;
my %ws;
foreach my $c (@class_keys) {
  $ws{$c} = {};
}
my $r = 1;
while ($r <= $loop) {
  warn "loop #$r\n";
  foreach my $d (@data) {
    foreach my $c (@class_keys) {
      my $w = $ws{$c};
      my $t = -1;
      if ($d->[0] eq $c) { $t = 1; }
      train($w, $d->[1], $t, $alpha, 1 / $r);
    }
  }
  $r++;
}


# invert weight
warn "invert weight.\n";
my %ii;
foreach my $c (@class_keys) {
  my $w = $ws{$c};
  foreach my $f (keys %$w) {
    $ii{$f}{$c} += $w->{$f};
  }
}


# print weight
warn "print weight.\n";
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
  my $r = shift;

  my $y = predict($w, $x);
  if ($y * $t < 1) {
    foreach my $f (keys %$x) {
      if (not $w->{$f}) { $w->{$f} = 0.0; }
      $w->{$f} += ($r * (($t * $x->{$f}) - $a * $w->{$f}));
    }
  }
}


