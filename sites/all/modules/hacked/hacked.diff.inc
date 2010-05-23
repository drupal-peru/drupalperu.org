<?php

// $Id: hacked.diff.inc,v 1.4 2010/02/28 08:28:36 darthsteven Exp $

function hacked_reports_hacked_diff($project, $file) {

  if (!module_exists('diff')) {
    return t('The diff module is required to use this feature.');
  }

  // Find a better way to do this:
  $breadcrumb = array(
    l('Home', '<front>'),
    l('Administer', 'admin'),
    l('Reports', 'admin/reports'),
    l('Hacked', 'admin/reports/hacked'),
    l($project['title'], 'admin/reports/hacked/' . $project['short_name']),
  );
  drupal_set_breadcrumb($breadcrumb);

  $local_file = hacked_find_local_project_directory($project) . '/' . $file;
  $this_release = $project['releases'][$project['existing_version']];

  // Let's see if there's a download link:
  $dir = hacked_download_release($this_release['download_link'], $project['project_type'], $project['short_name'], $project['existing_version']);


  // Special handling for core:
  if ($project['project_type'] == 'core') {
    $original_file = $dir . '/' . $project['short_name'] . '-' . $project['existing_version'] . '/' . $file;
  }
  else {
    $original_file = $dir . '/' . $project['short_name'] . '/' . $file;
  }

  if (!hacked_file_is_binary($original_file) && !hacked_file_is_binary($local_file)) {
    return hacked_diff_changed($local_file, $original_file);
  }
  //
  return t('Cannot hash binary file or file not found: %file', array('%file' => $file));
}

function hacked_diff_changed($installed_file, $original_file) {
  // Load up the two files and diff them:
  module_load_include('php', 'diff', 'DiffEngine');
  $formatter = new DrupalDiffFormatter();

  $original_array = file_exists($original_file) ? file($original_file) : array();
  $installed_array = file_exists($installed_file) ? file($installed_file) : array();

  $diff = new Diff($original_array, $installed_array);
  $output = theme('diff_table', array(t('Orignal'), '', t('Current'), ''), $formatter->format($diff), array('class' => 'diff hacked-diff'));
  return $output;

}
