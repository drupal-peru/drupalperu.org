<?php

// $Id: hacked.details.inc,v 1.3 2009/12/20 08:47:33 darthsteven Exp $

function hacked_reports_hacked_details($project) {
  $short_name = $project['short_name'];
  $projects = hacked_calculate_project_data(array($short_name => $project));

  // Sort the results:
  arsort($projects[$short_name]['hacked_results']);


  // Send the results to the theme function:
  $out = theme('hacked_detailed_report', $projects);

  return $out;
}

/**
 * Theme project status report.
 *
 * @ingroup themeable
 */
function theme_hacked_detailed_report($projects) {
  foreach ($projects as $project) {
    foreach ($project['hacked_results'] as $file => $status) {
      switch ($status) {
        case HACKED_STATUS_UNHACKED:
          $class = 'ok';
          $icon = theme('image', 'misc/watchdog-ok.png', t('Unchanged'), t('Unchanged'));
          break;
        case HACKED_STATUS_DELETED:
          $class = 'error';
          $icon = theme('image', 'misc/watchdog-error.png', t('Changed'), t('Changed'));
          break;
        case HACKED_STATUS_HACKED:
          $class = 'warning';
          $icon = theme('image', 'misc/watchdog-warning.png', t('Deleted'), t('Deleted'));
          break;
        case HACKED_STATUS_PERMISSION_DENIED:
          $class = 'warning';
          $icon = theme('image', 'misc/watchdog-warning.png', t('Permission Denied'), t('Permission Denied'));
          break;
        case HACKED_STATUS_UNCHECKED:
        default:
          $class = 'warning';
          $icon = theme('image', 'misc/watchdog-warning.png', t('Unchecked'), t('Unchecked'));
          break;
      }

      $row = '<div class="version-status">';
      switch ($status) {
        case HACKED_STATUS_UNHACKED:
          $row .= t('Unchanged');
          break;
        case HACKED_STATUS_HACKED:
          $row .= '<span class="not-current">' . t('Changed!') . '</span>';
          break;
        case HACKED_STATUS_DELETED:
          $row .= '<span class="">' . t('Deleted') . '</span>';
          break;
        case HACKED_STATUS_PERMISSION_DENIED:
          $row .= '<span class="">' . t('Permission Denied') . '</span>';
          break;
        case HACKED_STATUS_UNCHECKED:
        default:
          $row .= '<span class="">' . t('Unchecked') . '</span>';
          break;
      }
      $row .= '<span class="icon">' . $icon . '</span>';
      $row .= "</div>\n";

      $row .= '<div class="project">';
      if (user_access('view diffs of changed files') && module_exists('diff') && ($status != HACKED_STATUS_UNHACKED) && !hacked_file_is_binary(hacked_find_local_project_directory($project) . '/' . $file)) {
        $row .= l($file, 'admin/reports/hacked/' . $project['short_name'] . '/diff/' . $file);
      }
      else {
        $row .= check_plain($file);
      }
      $row .= "</div>\n";

      $row .= "</div>\n";

      $rows[] = array(
        'class' => $class,
        'data' => array($row),
      );
    }
    drupal_add_css(drupal_get_path('module', 'update') . '/update.css');
    return theme('table', NULL, $rows, array('class' => 'update'));
  }
  return 'here';
}
