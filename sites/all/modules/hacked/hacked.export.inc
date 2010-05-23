<?php

// $Id: hacked.export.inc,v 1.1 2009/12/20 08:50:20 darthsteven Exp $

function hacked_reports_export_select() {
  global $user;
  // Lookup the clean hashes in the cache table and offer to export them:
  $rows = array();
  $header = array(

  );

  $cache_results = db_query("SELECT cid, data, created, headers, expire, serialized FROM {". HACKED_CACHE_TABLE ."} WHERE cid LIKE '%s%%'", 'hacked:clean:hashes:');
  while ($cache = db_fetch_object($cache_results)) {
    if (isset($cache->data)) {
      // If the data is permanent or we're not enforcing a minimum cache lifetime
      // always return the cached data.
      if ($cache->expire == CACHE_PERMANENT || !variable_get('cache_lifetime', 0)) {
        $cache->data = db_decode_blob($cache->data);
        if ($cache->serialized) {
          $cache->data = unserialize($cache->data);
        }
      }
      // If enforcing a minimum cache lifetime, validate that the data is
      // currently valid for this user before we return it by making sure the
      // cache entry was created before the timestamp in the current session's
      // cache timer. The cache variable is loaded into the $user object by
      // sess_read() in session.inc.
      else {
        if ($user->cache > $cache->created) {
          // This cache data is too old and thus not valid for us, ignore it.
          continue;
        }
        else {
          $cache->data = db_decode_blob($cache->data);
          if ($cache->serialized) {
            $cache->data = unserialize($cache->data);
          }
        }
      }
      // Process the row:
      $key_split = explode(':', $cache->cid);
      if (count($key_split) == 6) {
        $row = array();
        $row[] = t('!type - !name - !version', array(
          '!type' => $key_split[3],
          '!name' => $key_split[4],
          '!version' => $key_split[5],
        ));
        $row[] = l(t('Export'), 'admin/reports/hacked-export/' . $cache->cid);

        $rows[] = $row;
      }

    }
  }


  return theme('table', $header, $rows);
}

function hacked_reports_exports_export_form($form_state, $cid) {
  $form = array();

  // Get the data from the cache:
  $cache = cache_get($cid, HACKED_CACHE_TABLE);
  if (!$cache->data) {
    return drupal_not_found();
  }

  $export = var_export($cache->data, TRUE);

  // Let's make it useful, valid PHP:
  $export = '$hashes = ' . $export;
  $export .= ';';

  $form['export'] = array(
    '#type' => 'textarea',
    '#default_value' => $export,
    '#rows' => count(explode("\n", $export)),
  );

  return $form;
}
