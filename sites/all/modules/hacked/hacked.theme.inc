<?php

// $Id: hacked.theme.inc,v 1.3 2009/12/20 08:47:33 darthsteven Exp $

/**
 * Theme project status report.
 *
 * @ingroup themeable
 */
function theme_hacked_report($data) {
  $output = '';

  if (!is_array($data)) {
    $output .= '<p>' . $data . '</p>';
    return $output;
  }

  $header = array();
  $rows = array();

  foreach ($data as $project) {
    if (!isset($project['hacked_status'])) {
      continue;
    }

    switch ($project['hacked_status']) {
      case HACKED_STATUS_UNHACKED:
        $class = 'ok';
        $icon = theme('image', 'misc/watchdog-ok.png', t('Unchanged'), t('Unchanged'));
        break;
      case HACKED_STATUS_HACKED:
        $class = 'error';
        $icon = theme('image', 'misc/watchdog-error.png', t('Changed'), t('Changed'));
        break;
      case HACKED_STATUS_UNCHECKED:
      default:
        $class = 'warning';
        $icon = theme('image', 'misc/watchdog-warning.png', t('Unchecked'), t('Unchecked'));
        break;
    }

    $row = '<div class="version-status">';
    switch ($project['hacked_status']) {
      case HACKED_STATUS_UNHACKED:
        $row .= t('Unchanged');
        break;
      case HACKED_STATUS_HACKED:
        $row .= '<span class="not-current">' . t('Changed!') . '</span>';
        break;
      case HACKED_STATUS_UNCHECKED:
      default:
        $row .= '<span class="">' . t('Unchecked') . '</span>';
        break;
    }
    $row .= '<span class="icon">' . $icon . '</span>';
    $row .= "</div>\n";

    $row .= '<div class="project">';
    if (isset($project['title'])) {
      if (isset($project['link'])) {
        $row .= l($project['title'], $project['link']);
      }
      else {
        $row .= check_plain($project['title']);
      }
    }
    else {
      $row .= check_plain($project['name']);
    }
    $row .= ' ' . check_plain($project['existing_version']);
    if ($project['install_type'] == 'dev' && !empty($project['datestamp'])) {
      $row .= ' <span class="version-date">(' . format_date($project['datestamp'], 'custom', 'Y-M-d') . ')</span>';
    }
    $row .= "</div>\n";


    $row .= "<div class=\"versions\">\n";
    $row .= '<table class="version version-latest">';
    $row .= '<tr>';

    $unreadable_message = "";
    if ($project['unreadable_count'] > 0) {
      $unreadable_message = ', ' . format_plural($project['unreadable_count'], '1 unreadable file', '@count unreadable files');
    }

    $row .= '<td class="version-title">'. format_plural($project['changed_count'], '1 file changed', '@count files changed') . ', ' . format_plural($project['deleted_count'], '1 file deleted', '@count files deleted') . $unreadable_message . "</td>\n";
    $row .= '</tr>';
    $row .= '</table>';
    $row .= '<table class="version version-latest">';
    $row .= '<tr>';
    $row .= '<td class="version-title">'. l(t('View details of changes'), 'admin/reports/hacked/' . $project['short_name']) ."</td>\n";
    $row .= '</tr>';

    $row .= '</table>';
    $row .= "</div>\n";

    /*
    if (isset($project['recommended'])) {
      if ($project['status'] != UPDATE_CURRENT || $project['existing_version'] !== $project['recommended']) {

        // First, figure out what to recommend.
        // If there's only 1 security update and it has the same version we're
        // recommending, give it the same CSS class as if it was recommended,
        // but don't print out a separate "Recommended" line for this project.
        if (!empty($project['security updates']) && count($project['security updates']) == 1 && $project['security updates'][0]['version'] === $project['recommended']) {
          $security_class = ' version-recommended version-recommended-strong';
        }
        else {
          $security_class = '';
          $version_class = 'version-recommended';
          // Apply an extra class if we're displaying both a recommended
          // version and anything else for an extra visual hint.
          if ($project['recommended'] !== $project['latest_version']
              || !empty($project['also'])
              || ($project['install_type'] == 'dev'
                 && isset($project['dev_version'])
                 && $project['latest_version'] !== $project['dev_version']
                 && $project['recommended'] !== $project['dev_version'])
              || (isset($project['security updates'][0])
                 && $project['recommended'] !== $project['security updates'][0])
              ) {
            $version_class .= ' version-recommended-strong';
          }
          $row .= theme('update_version', $project['releases'][$project['recommended']], t('Recommended version:'), $version_class);
        }

        // Now, print any security updates.
        if (!empty($project['security updates'])) {
          foreach ($project['security updates'] as $security_update) {
            $row .= theme('update_version', $security_update, t('Security update:'), 'version-security'. $security_class);
          }
        }
      }

      if ($project['recommended'] !== $project['latest_version']) {
        $row .= theme('update_version', $project['releases'][$project['latest_version']], t('Latest version:'), 'version-latest');
      }
      if ($project['install_type'] == 'dev'
          && $project['status'] != UPDATE_CURRENT
          && isset($project['dev_version'])
          && $project['recommended'] !== $project['dev_version']) {
        $row .= theme('update_version', $project['releases'][$project['dev_version']], t('Development version:'), 'version-latest');
      }
    }

    if (isset($project['also'])) {
      foreach ($project['also'] as $also) {
        $row .= theme('update_version', $project['releases'][$also], t('Also available:'), 'version-also-available');
      }
    }

    $row .= "</div>\n"; // versions div.
    */

    $row .= "<div class=\"info\">\n";
    if (!empty($project['extra'])) {
      $row .= '<div class="extra">' . "\n";
      foreach ($project['extra'] as $key => $value) {
        $row .= '<div class="' . $value['class'] . '">';
        $row .= check_plain($value['label']) . ': ';
        $row .= theme('placeholder', $value['data']);
        $row .= "</div>\n";
      }
      $row .= "</div>\n";  // extra div.
    }

    $row .= '<div class="includes">';
    sort($project['includes']);
    $row .= t('Includes: %includes', array('%includes' => implode(', ', $project['includes'])));
    $row .= "</div>\n";

    $row .= "</div>\n"; // info div.

    if (!isset($rows[$project['project_type']])) {
      $rows[$project['project_type']] = array();
    }
    $rows[$project['project_type']][] = array(
      'class' => $class,
      'data' => array($row),
    );
  }

  $project_types = array(
    'core' => t('Drupal core'),
    'module' => t('Modules'),
    'theme' => t('Themes'),
    'disabled-module' => t('Disabled modules'),
    'disabled-theme' => t('Disabled themes'),
  );
  foreach ($project_types as $type_name => $type_label) {
    if (!empty($rows[$type_name])) {
      $output .= "\n<h3>" . $type_label . "</h3>\n";
      $output .= theme('table', $header, $rows[$type_name], array('class' => 'update'));
    }
  }
  drupal_add_css(drupal_get_path('module', 'update') . '/update.css');
  return $output;
}
