#!/usr/bin/env perl
use strict;
use warnings;

# read file, and calculate tf & idf
warn "read file, and calculate tf & idf.\n";
my %tf;
my %df;
my $N = 1;
while (<>) {
  warn "#$.\n" unless ($. % 10000);
  chomp;
  my @a = split(/\t/);
  if (@a != 2) { warn "#$. is invalid. ignored.\n"; next; }
  my $class_key = $a[0];
  $class_key =~ s/(:.+)//;

  my @b = split(/,/, $a[1]);
  foreach (@b) {
    my @c = split(/:/);
    if (@c != 2) { warn "#$. ($_) is invalid. ignored.\n"; next; }
    $tf{$c[0]}{$class_key} += $c[1];
    $df{$c[0]}++;
  }
  $N++;
}


# calculate tfidf & class norm
warn "calculate tfidf & class norm.\n";
my %class_norm;
my $log_N = log($N);
foreach my $feature_key (keys %tf) {
  my $fr = $tf{$feature_key};

  my $log_df = $log_N - log($df{$feature_key});
  foreach my $class_key (keys %$fr) {
    my $tfidf = $fr->{$class_key} * $log_df;
    $class_norm{$class_key} += $tfidf;
#    $tfidf *= $log_df;
    $fr->{$class_key} = $tfidf;
  }
}


# print normalized tf-idf
warn "print normalized tf-idf.\n";
foreach my $feature_key (keys %tf) {
  my $fr = $tf{$feature_key};
  print "$feature_key\t";

  my $sep    = '';
  foreach my $class_key (keys %$fr) {
    my $normalized_tfidf = $fr->{$class_key} / $class_norm{$class_key};
    $normalized_tfidf = int($normalized_tfidf * 10000) / 10000;
    print "$sep$class_key:$normalized_tfidf";
    $sep = ',';
  }
  print "\n";
}

