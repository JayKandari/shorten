<?php

/**
 * @file
 *   Settings for the Shorten module.
 */

/**
 * Returns HTML representing the shorten admin form.
 */
function shorten_admin_form() {
  $form = \Drupal::formBuilder()->getForm('shorten_admin');
  return \Drupal::service("renderer")->render($form);
}

/**
 * Settings page.
 */
function shorten_admin($form, $form_state) {
  $form['shorten_www'] = array(
    '#type' => 'checkbox',
    '#title' => t('Use "www." instead of "http://"'),
    '#description' => t('"www." is shorter, but "http://" is automatically link-ified by more services.'),
    '#default_value' => \Drupal::config('shorten.settings')->get('shorten_www'),
  );
  $methods = array();
  if (function_exists('file_get_contents')) {
    $methods['php'] = t('PHP');
  }
  if (function_exists('curl_exec')) {
    $methods['curl'] = t('cURL');
  }
  if (empty($methods)) {
    // @FIXME
// Could not extract the default value because it is either indeterminate, or
// not scalar. You'll need to provide a default value in
// config/install/shorten.settings.yml and config/schema/shorten.schema.yml.
if (\Drupal::config('shorten.settings')->get('shorten_method') != 'none') {
      \Drupal::configFactory()->getEditable('shorten.settings')->set('shorten_method', 'none')->save();
    }
    $form['shorten_method'] = array(
      '#type' => 'radios',
      '#title' => t('Method'),
      '#description' => '<p>' . t('The method to use to retrieve the abbreviated URL.') . '</p>' .
        '<p><strong>' . t('Your PHP installation does not support the URL abbreviation feature of the Shorten module.') . '</strong> ' .
        t('You must compile PHP with either the cURL library or the file_get_contents() function to use this option.') . '</p>',
      '#default_value' => 'none',
      '#options' => array('none' => t('None')),
      '#disabled' => TRUE,
    );
    $form['shorten_service'] = array(
      '#type' => 'radios',
      '#title' => t('Service'),
      '#description' => t('The default service to use to create the abbreviated URL.'),
      '#default_value' => 'none',
      '#options' => array('none' => t('None')),
    );
    $form['shorten_service_backup'] = array(
      '#type' => 'radios',
      '#title' => t('Backup Service'),
      '#description' => t('The service to use to create the abbreviated URL if the primary service is down.'),
      '#default_value' => 'none',
      '#options' => array('none' => t('None')),
    );
  }
  else {
    // @FIXME
// Could not extract the default value because it is either indeterminate, or
// not scalar. You'll need to provide a default value in
// config/install/shorten.settings.yml and config/schema/shorten.schema.yml.
$form['shorten_method'] = array(
      '#type' => 'radios',
      '#title' => t('Method'),
      '#description' => t('The method to use to retrieve the abbreviated URL. cURL is much faster, if available.'),
      '#default_value' => \Drupal::config('shorten.settings')->get('shorten_method'),
      '#options' => $methods,
    );
    $all_services = \Drupal::moduleHandler()->invokeAll('shorten_service');
    $services = array();
    foreach ($all_services as $key => $value) {
      $services[$key] = $key;
    }
    $services['none'] = t('None');
    $form['shorten_service'] = array(
      '#type' => 'select',
      '#title' => t('Service'),
      '#description' => t('The default service to use to create the abbreviated URL.') .' '.
        t('If a service is not shown in this list, you probably need to configure it in the Shorten API Keys tab.'),
      '#default_value' => \Drupal::config('shorten.settings')->get('shorten_service'),
      '#options' => $services,
    );
    $form['shorten_service_backup'] = array(
      '#type' => 'select',
      '#title' => t('Backup Service'),
      '#description' => t('The service to use to create the abbreviated URL if the primary or requested service is down.'),
      '#default_value' => \Drupal::config('shorten.settings')->get('shorten_service_backup'),
      '#options' => $services,
    );
    $form['shorten_show_service'] = array(
      '#type' => 'checkbox',
      '#title' => t('Show the list of URL shortening services in the user interface'),
      '#default_value' => \Drupal::config('shorten.settings')->get('shorten_show_service'),
      '#description' => t('Allow users to choose which service to use in the Shorten URLs block and page.'),
    );
  }
  $form['shorten_use_alias'] = array(
    '#type' => 'checkbox',
    '#title' => t('Shorten aliased URLs where possible'),
    '#description' => t('Where possible, generate shortened URLs based on the aliased version of a URL.')
      . ' <strong>' . t('Some integrated modules ignore this.') . '</strong>',
    '#default_value' => \Drupal::config('shorten.settings')->get('shorten_use_alias'),
  );
  $form['shorten_timeout'] = array(
    '#type' => 'textfield',
    '#title' => t('Time out URL shortening requests after'),
    '#field_suffix' => ' ' . t('seconds'),
    '#description' => t('Cancel retrieving a shortened URL if the URL shortening service takes longer than this amount of time to respond.') . ' ' .
      t('Lower values (or shorter timeouts) mean your site will respond more quickly if your URL shortening service is down.') . ' ' .
      t('However, higher values (or longer timeouts) give the URL shortening service more of a chance to return a value.') . ' ' .
      t('If a request to the primary service times out, the secondary service is used. If the secondary service times out, the original (long) URL is used.') . ' ' .
      t('You must enter a nonnegative integer. Enter 0 (zero) to wait for a response indefinitely.'),
    '#size' => 3,
    '#required' => TRUE,
    '#default_value' => \Drupal::config('shorten.settings')->get('shorten_timeout'),
  );
  $form['shorten_cache_duration'] = array(
    '#type' => 'textfield',
    '#title' => t('Cache shortened URLs for'),
    '#field_suffix' => ' ' . t('seconds'),
    '#description' => t('Shortened URLs are stored after retrieval to improve performance.') . ' ' .
      t('Enter the number of seconds for which you would like the shortened URLs to be stored.') . ' ' .
      t('Leave this field blank to store shortened URLs indefinitely (although this is not recommended).') . ' ' .
      t('The default value is 1814400 (3 weeks).'),
    '#size' => 11,
    '#default_value' => \Drupal::config('shorten.settings')->get('shorten_cache_duration'),
  );
  $form['shorten_cache_fail_duration'] = array(
    '#type' => 'textfield',
    '#title' => t('On failure, cache full URLs for'),
    '#field_suffix' => ' ' . t('seconds'),
    '#description' => t('When a shortener service is unavilable, the full URL will be cached temporarily to prevent more requests from overloading the server.') .' '.
      t('Enter the number of seconds for which you would like to store these full URLs when shortening the URL fails.') .' '.
      t('The default value is 1800 (30 minutes).'),
    '#size' => 11,
    '#required' => TRUE,
    '#default_value' => \Drupal::config('shorten.settings')->get('shorten_cache_fail_duration'),
  );
  $form['shorten_cache_clear_all'] = array(
    '#type' => 'checkbox',
    '#title' => t('Clear Shorten URLs cache when all Drupal caches are cleared.'),
    '#description' => t('Sometimes Drupal automatically clears all caches, such as after running database updates.') . ' ' .
      t('However, regenerating the cache of shortened URLs can be performance-intensive, and the cache does not affect Drupal behaviors.') . ' ' .
      t('To avoid regenerating this cache after clearing all Drupal caches, un-check this option.') . ' ' .
      t('Note that if you need to completely clear this cache, un-checking this option will require that you do it manually.'),
    '#default_value' => \Drupal::config('shorten.settings')->get('shorten_cache_clear_all'),
  );
  unset($services['none']);
  // @FIXME
// Could not extract the default value because it is either indeterminate, or
// not scalar. You'll need to provide a default value in
// config/install/shorten.settings.yml and config/schema/shorten.schema.yml.
$form['shorten_invisible_services'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Disallowed services'),
    '#description' => t('Checking the box next to a service will make it <strong>unavailable</strong> for use in the Shorten URLs block and page.') . ' ' .
      t('If you disallow all services, the primary service will be used.'),
    '#default_value' => \Drupal::config('shorten.settings')->get('shorten_invisible_services'),
    '#options' => array_map('check_plain', $services),
  );
  return system_settings_form($form);
}

/**
 * Validation handler for shorten_admin().
 */
function shorten_admin_validate($form, &$form_state) {
  $v = $form_state['values'];
  if ($v['shorten_service'] == $v['shorten_service_backup'] && $v['shorten_service_backup'] != 'none') {
    form_set_error('shorten_service_backup', t('You must select a backup abbreviation service that is different than your primary service.'));
  }
  elseif (($v['shorten_service'] == 'bit.ly' && $v['shorten_service_backup'] == 'j.mp') ||
    ($v['shorten_service'] == 'j.mp' && $v['shorten_service_backup'] == 'bit.ly')) {
    form_set_error('shorten_service_backup', t('j.mp and bit.ly are the same service.') . ' ' .
      t('You must select a backup abbreviation service that is different than your primary service.'));
  }
  if ($v['shorten_service'] == 'none' && $v['shorten_service_backup'] != 'none') {
    drupal_set_message(t('You have selected a backup URL abbreviation service, but no primary service.') . ' ' .
      t('Your URLs will not be abbreviated with these settings.'));
  }
  if ($v['shorten_cache_duration'] !== '' && (
    !is_numeric($v['shorten_cache_duration']) ||
    round($v['shorten_cache_duration']) != $v['shorten_cache_duration'] ||
    $v['shorten_cache_duration'] < 0
  )) {
    form_set_error('shorten_cache_duration', t('The cache duration must be a positive integer or left blank.'));
  }
  if (
    !is_numeric($v['shorten_cache_fail_duration']) ||
    round($v['shorten_cache_fail_duration']) != $v['shorten_cache_fail_duration'] ||
    $v['shorten_cache_fail_duration'] < 0
  ) {
    form_set_error('shorten_cache_fail_duration', t('The cache fail duration must be a positive integer.'));
  }
  if (!is_numeric($v['shorten_timeout']) || round($v['shorten_timeout']) != $v['shorten_timeout'] || $v['shorten_timeout'] < 0) {
    form_set_error('shorten_timeout', t('The timeout duration must be a nonnegative integer.'));
  }
}

/**
 * Submit handler for shorten_admin().
 */
function shorten_admin_submit($form, &$form_state) {
  // Changed settings usually mean that different URLs should be used.
  cache_clear_all('*', 'cache_shorten', TRUE);
}

/**
 * Returns HTML representing the shorten keys admin form.
 */
function shorten_keys_form() {
  $form = \Drupal::formBuilder()->getForm('shorten_keys');
  return \Drupal::service("renderer")->render($form);
}

/**
 * Keys page.
 */
function shorten_keys($form, $form_state) {
  $form['shorten_bitly'] = array(
    '#type' => 'fieldset',
    '#title' => t('Bit.ly and j.mp'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );
  $form['shorten_bitly']['shorten_bitly_login'] = array(
    '#type' => 'textfield',
    '#title' => t('Bit.ly Login'),
    '#default_value' => \Drupal::config('shorten.settings')->get('shorten_bitly_login'),
  );
  $form['shorten_bitly']['shorten_bitly_key'] = array(
    '#type' => 'textfield',
    '#title' => t('Bit.ly API Key'),
    '#default_value' => \Drupal::config('shorten.settings')->get('shorten_bitly_key'),
  );
  $form['shorten_budurl'] = array(
    '#type' => 'textfield',
    '#title' => t('BudURL API Key'),
    '#default_value' => \Drupal::config('shorten.settings')->get('shorten_budurl'),
  );
  $form['shorten_cligs'] = array(
    '#type' => 'textfield',
    '#title' => t('Cligs API Key'),
    '#default_value' => \Drupal::config('shorten.settings')->get('shorten_cligs'),
  );
  $form['shorten_ez'] = array(
    '#type' => 'textfield',
    '#title' => t('Ez API Key'),
    '#default_value' => \Drupal::config('shorten.settings')->get('shorten_ez'),
  );
  $form['shorten_fwd4me'] = array(
    '#type' => 'textfield',
    '#title' => t('Fwd4.me API Key'),
    '#default_value' => \Drupal::config('shorten.settings')->get('shorten_fwd4me'),
  );
  $form['shorten_googl'] = array(
    '#type' => 'textfield',
    '#title' => t('Goo.gl API Key'),
    '#default_value' => \Drupal::config('shorten.settings')->get('shorten_googl'),
  );
  $form['shorten_redirec'] = array(
    '#type' => 'textfield',
    '#title' => t('Redir.ec API Key'),
    '#default_value' => \Drupal::config('shorten.settings')->get('shorten_redirec'),
  );
  return system_settings_form($form);
}
