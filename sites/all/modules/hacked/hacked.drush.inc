<?php

// $Id: hacked.drush.inc,v 1.9 2010/04/23 14:41:57 darthsteven Exp $

/**
 * @file
 *   Hacked drush command.
 *
 *   Enables drush support for the Hacked! module.
 */

/**
 * Implementation of hook_drush_help().
 */
function hacked_drush_help($section) {
  switch ($section) {
    case 'drush:hacked-list-projects':
      return dt('List projects and their hacked/unhacked status.');
    case 'drush:hacked-details':
      return dt('Show details of the files in one project, and the hacked/unhacked status of those files.');
    case 'drush:hacked-diff':
      return dt('Output a unified diff of the specified project.');

  }
}

/**
 * Implementation of hook_drush_command().
 *
 * @See drush_parse_command() for a list of recognized keys.
 *
 * @return
 *   An associative array describing your command(s).
 */
function hacked_drush_command() {
  $items = array();

  $items['hacked-list-projects'] = array(
    'description' => "List all projects that can be analysed by Hacked! ",
    'drupal dependencies' => array('hacked'),
  );

  $items['hacked-details'] = array(
    'description' => "Show the Hacked! report about a specific project.",
    'drupal dependencies' => array('hacked'),
    'arguments' => array(
      'project' => 'The machine name of the project to report on.',
    ),
  );

  $items['hacked-diff'] = array(
    'description' => "Output a unified diff of the project specified.",
    'drupal dependencies' => array('hacked'),
    'arguments' => array(
      'project' => 'The machine name of the project to report on.',
    ),
  );

  return $items;
}

/**
 * Drush command callback that shows the listing of changed/unchanged projects.
 */
function drush_hacked_list_projects() {

  // Go get the data:
  module_load_include('inc', 'update', 'update.report');
  if ($available = update_get_available(TRUE)) {
    module_load_include('inc', 'update', 'update.compare');
    $data = update_calculate_project_data($available);
    $data = hacked_calculate_project_data($data);
    // Now print the data using drush:
    $rows[] = array(
      dt('Project'),
      dt('Shortname'),
      dt('Version'),
      dt('Status'),
      dt('Changed'),
      dt('Deleted'),
    );
    foreach ($data as $project) {
      $row = array(
        empty($project['title']) ? $project['name'] : $project['title'],
        $project['name'],
        $project['existing_version']
      );

      // Now add the status:
      switch ($project['hacked_status']) {
        case HACKED_STATUS_UNHACKED:
          $row[] = dt('Unchanged');
          break;
        case HACKED_STATUS_HACKED:
          $row[] = t('Changed');
          break;
        case HACKED_STATUS_UNCHECKED:
        default:
          $row[] = t('Unchecked');
          break;
      }

      $row[] = $project['changed_count'];
      $row[] = $project['deleted_count'];


      $rows[] = $row;
    }
    drush_print_table($rows, TRUE);

  }


}

/**
 * Drush command callback that shows the list of changes/unchanged files in a project.
 *
 * You may specify the --include-unchanged option to show unchanged files too,
 * otherwise just the changed and deleted files are shown.
 */
function drush_hacked_details($short_name = '') {
  $project = hacked_project_load($short_name);
  if (!$project) {
    return drush_set_error('HACKED_PROJECT_NOT_FOUND', dt('Could not find project: !project', array('!project' => $short_name)));
  }

  drush_print(dt('Details for project: @name', array('@name' => $project['title'])));
  drush_print(dt('Total files: @total_files, files changed: @changed_files, deleted files: @deleted_files', array(
    '@total_files' => count($project['hacked_results']),
    '@changed_files' => $project['changed_count'],
    '@deleted_files' => $project['deleted_count'],

  )));
  drush_print('');

  drush_print(dt('Detailed results:'));
  // Sort the results:
  arsort($project['hacked_results']);

  $rows[] = array(
    dt('Status'),
    dt('File'),
  );
  $show_unchanged = drush_get_option('include-unchanged', FALSE);
  foreach ($project['hacked_results'] as $file => $status) {
    if (!$show_unchanged && $status == HACKED_STATUS_UNHACKED) {
      continue;
    }
    $row = array(
    );

    // Now add the status:
    switch ($status) {
      case HACKED_STATUS_UNHACKED:
        $row[] = dt('Unchanged');
        break;
      case HACKED_STATUS_HACKED:
        $row[] = t('Changed');
        break;
      case HACKED_STATUS_DELETED:
        $row[] = t('Deleted');
        break;
      case HACKED_STATUS_UNCHECKED:
      default:
        $row[] = t('Unchecked');
        break;
    }

    $row[] = $file;


    $rows[] = $row;
  }
  drush_print_table($rows, TRUE);


}

/**
 * Drush command callback that shows the list of changes/unchanged files in a project.
 *
 * You may specify the --include-unchanged option to show unchanged files too,
 * otherwise just the changed and deleted files are shown.
 */
function drush_hacked_diff($short_name = '') {
  $project = hacked_project_load($short_name);
  if (!$project) {
    return drush_set_error('HACKED_PROJECT_NOT_FOUND', dt('Could not find project: !project', array('!project' => $short_name)));
  }
  $local_location = realpath(hacked_find_local_project_directory($project));
  $clean_location = hacked_download_release($this_release['download_link'], $project['project_type'], $project['short_name'], $project['existing_version']);
  // More special handling for core:
  if ($project['type'] != 'core') {
    $clean_location = $clean_location . "/${project['short_name']}";
  }
  else {
    $clean_location = $clean_location . "/${project['short_name']}-${project['version']}";
  }
  $clean_location = realpath($clean_location);
  drush_shell_exec("diff -upr $clean_location $local_location");

  $lines = drush_shell_exec_output();
  $local_location_trim = dirname($local_location . '/dummy.file') . '/';
  $clean_location_trim = dirname($clean_location . '/dummy.file') . '/';
  foreach ($lines as $line) {
    if (strpos($line, '+++') === 0) {
      $line = str_replace($local_location_trim, '', $line);
    }
    if (strpos($line, '---') === 0) {
      $line = str_replace($clean_location_trim, '', $line);
    }
    if (strpos($line, 'diff -upr') === 0) {
      $line = str_replace($clean_location_trim, 'old/', $line);
      $line = str_replace($local_location_trim, 'new/', $line);
    }

    drush_print($line);

  }

}
