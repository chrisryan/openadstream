<?php
// $Id: openadstream.admin.inc,v 1.1.2.16 2010/01/12 04:14:44 damienmckenna Exp $
/**
 * @file
 * Settings pages for Open AdStream.
 */

/**
 * Main settings page.
 */
function openadstream_admin_settings() {
  $form = array();
  
  $form['main_settings'] = array(
    '#type' => 'fieldset',
    '#title' => t('General Settings'),
    '#collapsible' => FALSE,
    '#collapsed' => FALSE,
    '#description' => t("These settings should be provided by 24/7 RealMedia."),
  );

  $form['main_settings']['openadstream_oas_hostname'] = array(
    '#type' => 'textfield',
    '#title' => t('Open AdStream Hostname'),
    '#default_value' => variable_get('openadstream_oas_hostname', 'oascentral.247realmedia.com'),
    '#size' => 50,
    '#maxlength' => 100,
    '#description' => 'The hostname used to request ads, will be provided by 24/7 Real Media.  Do not include the initial "http://" nor any slashes, just the bare hostname.',
    '#required' => TRUE,
  );

  $form['main_settings']['openadstream_hostname'] = array(
    '#type' => 'textfield',
    '#title' => t('Site Hostname'),
    '#default_value' => variable_get('openadstream_hostname', ''),
    '#size' => 50,
    '#maxlength' => 100,
    '#description' => 'By default, we will be using the current hostname',
  );

  // Optional $_GET variable to override the pagename.
  $form['pagename_settings'] = array(
    '#type' => 'fieldset',
    '#title' => t('Pagename Settings (optional)'),
    '#collapsible' => FALSE,
    '#collapsed' => FALSE,
  );
  $form['pagename_settings']['openadstream_pagename'] = array(
    '#type' => 'textfield',
    '#title' => t('Fixed Page Name'),
    '#default_value' => variable_get('openadstream_pagename', ''),
    '#size' => 50,
    '#maxlength' => 50,
    '#description' => '(optional) All page requests will load the same pagename value. Only use this for testing, will produce unwanted results in a production environment.',
  );
  $form['pagename_settings']['openadstream_add_basepath'] = array(
    '#type' => 'checkbox',
    '#title' => t('Add Base Path'),
    '#default_value' => variable_get('openadstream_add_basepath', TRUE),
    '#description' => '(optional) By default the pagename variable will be prepended
      by the string provided by base_path().  In occasions when you are
      running the site within a subdirectory but do not want that directory
      name added, disable this setting.',
  );
  $form['pagename_settings']['openadstream_querystring'] = array(
    '#type' => 'textfield',
    '#title' => t('URL String'),
    '#default_value' => variable_get('openadstream_querystring', ''),
    '#size' => 10,
    '#maxlength' => 20,
    '#description' => '(optional) Override the configured pagename via a URL string.',
  );

  return system_settings_form($form);
}

/**
 * Gutter settings page.
 */
function openadstream_admin_gutters() {
  $form = array();
  
  // Grab an array of all positions.
  $positions = _openadstream_position_names();
  // Insert an empty element at the front to make the selection optional.
  array_unshift($positions, '');

  $form['gutter'] = array(
    '#type' => 'fieldset',
    '#title' => t('Main Gutter Ad Settings'),
    '#collapsible' => FALSE,
    '#collapsed' => FALSE,
    '#description' => t("Allows you to display ads in the page gutters, one ad per side.  For flexibility, the ads are not dependent upon each other, so you can display one but not the other."),
  );
  $form['gutter']['openadstream_gutter_enabled'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable Gutter Ads'),
    '#description' => t('Enable the gutter advertisements position.'),
    '#default_value' => variable_get('openadstream_gutter_enabled', FALSE),
  );
  $form['gutter']['openadstream_gutter_left'] = array(
    '#type' => 'select',
    '#title' => t('Left Gutter Position'),
    '#description' => t('Optional position to place in the left page gutter.'),
    '#options' => $positions,
    '#required' => FALSE,
    '#default_value' => variable_get('openadstream_gutter_left', ''),
  );
  $form['gutter']['openadstream_gutter_right'] = array(
    '#type' => 'select',
    '#title' => t('Right Gutter Position'),
    '#description' => t('Optional position to place in the right page gutter.'),
    '#options' => $positions,
    '#required' => FALSE,
    '#default_value' => variable_get('openadstream_gutter_right', ''),
  );
  $form['gutter']['openadstream_gutter_dom'] = array(
    '#type' => 'textfield',
    '#title' => t('DOM Element for Width'),
    '#description' => t('The DOM element to use to decide the page width, e.g. "#page".  This should be the outermost page element on your page, usually the first DIV inside your BODY tag.  This is identified using jQuery selection strings so you can get advanced with this, if needed.'),
    '#default_value' => variable_get('openadstream_gutter_dom', '#page'),
  );
  $form['gutter']['openadstream_gutter_padding'] = array(
    '#type' => 'select',
    '#title' => t('Padding (px)'),
    '#options' => range(0, 100),
    '#description' => t('Optional padding (in pixels) to show between the gutter and the page content.'),
    '#default_value' => variable_get('openadstream_gutter_padding', 0),
  );

  // Display the gutters - includes.
  $form = array_merge($form, openadstream_admin_gutters_list('include'));

  // Display the gutters - excludes.
  $form = array_merge($form, openadstream_admin_gutters_list('exclude'));

  // TODO: button to clear the cache.

  return system_settings_form($form);
}

function openadstream_admin_gutters_list($type) {
  // Load all gutters.
  static $gutters;
  if ($gutters === NULL) {
    $gutters = _openadstream_list_gutters_all();
  }

  $form = array();
  $form[$type . 's'] = array(
    '#type' => 'fieldset',
    '#title' => t('Paths to '. $type),
    '#collapsible' => FALSE,
    '#collapsed' => FALSE,
    '#description' => t(""),
  );
  $header = array(
    t('Path(s)'),
    t('Date From'),
    t('Date To'),
    t('BG Color'),
    t('Operations'),
  );

  $now = REQUEST_TIME;

  // Compile the gutter records.
  $rows = array();
  foreach ($gutters[$type] as $gutter) {
    // Some UI tweaks.
    $classes = array();
    // Format the From Date field.
    if (empty($gutter['date_from'])) {
      $date_from = "<em>" . t('immediately') . "</em>";
    }
    else {
      $date_from = format_date($gutter['date_from'], 'short');
      if ($gutter['date_from'] > $now) {
        $classes[] = 'inactive';
        $date_from .= ' - <em>' . t('inactive') . '</em>';
      }
    }
    // Format the To Date field.
    if (empty($gutter['date_to'])) {
      $date_to = '<em>' . t('no expiration') . '</em>';
    }
    else {
      $date_to = format_date($gutter['date_to'], 'short');
      if ($gutter['date_to'] < $now) {
        $classes[] = 'expired';
        $date_to .= ' - <em>' . t('expired!') . '</em>';
      }
    }
    // Compile the row.
    $rows[] = array(
      'data' => array(
        'path' => nl2br($gutter['path']),
        'date_from' => $date_from,
        'date_to' => $date_to,
        'bgcolor' => $gutter['bgcolor'],
        'actions' => l('edit', 'admin/build/openadstream/gutters-edit/' . $gutter['id'])
          . ', ' . l('delete', 'admin/build/openadstream/gutters-delete/' . $gutter['id']),
      ),
      'class' => $classes,
    );
  }
  $form[$type . 's']['table'] = array(
    '#type' => 'markup',
    '#markup' => theme('table', array('header' => $header, 'rows' => $rows)),
  );
  
  return $form;
}

/**
 * Show the New Gutters Ad form.
 */
function openadstream_admin_gutters_add() {
  return openadstream_admin_gutters_form();
}

/**
 * Show the New Gutters Ad form.
 */
function openadstream_admin_gutters_edit($form, &$form_state) {
  $id = $form_state['build_info']['args'][0];
  $gutter = db_query("SELECT g.id, g.path, g.date_from, g.date_to, g.is_include FROM {openadstream_gutters} g WHERE id=:id", array(':id' => $id))->fetchAssoc();
  return openadstream_admin_gutters_form($gutter);
}

/**
 * Build the Gutter Ad path form.
 */
function openadstream_admin_gutters_form($gutter = array()) {
  $form = array();

  // If this is an edit.
  if (is_array($gutter) && isset($gutter['id'])) {
    $form['id'] = array(
      '#type' => 'value',
      '#value' => $gutter['id'],
    );
    if (!empty($gutter['date_from'])) {
      $gutter['date_from'] = date_format_date(new DateObject($gutter['date_from']), DATE_DATETIME);
    }
    if (!empty($gutter['date_to'])) {
      $gutter['date_to'] = date_format_date(new DateObject($gutter['date_to']), DATE_DATETIME);
    }
  }
  // Assign some defaults.
  else {
    $gutter = array(
      'is_include' => '1',
      'path' => NULL,
      'date_from' => NULL,
      'date_to' => NULL,
      'bgcolor' => NULL,
    );
  }

  $form['gutter'] = array(
    '#type' => 'fieldset',
    '#collapsible' => FALSE,
    '#collapsed' => FALSE,
    '#title' => t('Path Settings'),
    '#description' => t('Paths that should be included or excluded; this follows the standard Block Visiblity Rules style:
    <ul><li>One per line.</li><li>"*" may be used as a wildcard.</li></ul>
    Examples:
    <ul><li>"product*" will match the paths "product", "products" and "products/purple-people-eater".</li></ul>
    Paths will be first matched to see if they are included and then matched against the exclude list.'),
  );
  $form['gutter']['is_include'] = array(
    '#type' => 'radios',
    '#title' => t('What to do'),
    '#options' => array('1' => 'Include these paths', '0' => 'Exclude these paths'),
    '#default_value' => $gutter['is_include'],
  );
  $form['gutter']['path'] = array(
    '#type' => 'textarea',
    '#title' => t('Paths to Include'),
    '#default_value' => $gutter['path'],
    '#required' => TRUE,
  );
  $form['date_fields'] = array(
    '#type' => 'fieldset',
    '#collapsible' => FALSE,
    '#collapsed' => FALSE,
    '#title' => t('Date Range (optional)'),
    '#description' => 'These may be combined to create specific effects, e.g. for ads that should start immediately and end on a specific date you should leave the "From" date blank and assign the "Until" date as needed.',
  );
  $form['date_fields']['date_from'] = array(
    '#type' => 'date_select',
    '#title' => t('From'),
    '#default_value' => $gutter['date_from'],
    '#required' => FALSE,
    '#date_year_range' => '-1:+3',
    '#description' => 'The date the gutter ads will be visible on the above paths.  Optional.',
  );
  $form['date_fields']['date_to'] = array(
    '#type' => 'date_select',
    '#title' => t('Until'),
    '#default_value' => $gutter['date_to'],
    '#required' => FALSE,
    '#date_year_range' => '-1:+3',
    '#description' => 'The date the gutter ads stop showing.  Optional.',
  );
  $form['background_color'] = array(
    '#type' => 'fieldset',
    '#collapsible' => FALSE,
    '#collapsed' => FALSE,
    '#title' => t('Page Background Color (optional)'),
    '#description' => '',
  );
  $form['background_color']['bgcolor'] = array(
    '#type' => (module_exists('colorpicker') ? 'colorpicker_' : '') . 'textfield',
    '#title' => t('HEX Color Code'),
    '#default_value' => (isset($gutter['bgcolor']) ? $gutter['bgcolor'] : ''),
    '#required' => FALSE,
    '#maxlength' => '7',
    '#size' => '10',
    '#description' => 'The color code for the page background.  This is only applicable for paths that are included.  Optional.',
  );
  
  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => 'Save'
  );
  
  return $form;
}

/**
 * Validate the data submitted in the Gutter Ad form.
 */
function openadstream_admin_gutters_add_validate($form, &$form_state) {
  $values = $form_state['values'];
  
  // Make sure the From date is not after the Until date.
  if (!empty($values['date_to']) && !empty($values['date_to'])) {
    $date_from = date_timestamp_get(new DateObject($values['date_from']));
    $date_to = date_timestamp_get(new DateObject($values['date_from']));
    
    if ($date_from > $date_to) {
      form_set_error('date_from', t('The "From" date should not be after the "Until" date'));
    }
  }
}

/**
 * Save the data submitted in the Gutter Ad form.
 */
function openadstream_admin_gutters_add_submit($form, &$form_state) {
  $values = $form_state['values'];

  // Compile the record.
  $obj = new stdClass;
  $obj->path = $values['path'];
  $obj->is_include = !!$values['is_include'];

  // Convert the date field to UNIX timestamp.
  if (!empty($values['date_from'])) {
    $values['date_from'] = date_timestamp_get(new DateObject($values['date_from']));
    $obj->date_from = $values['date_from'];
  }
  else {
    $obj->date_from = 0;
  }

  // Convert the date field to UNIX timestamp.
  if (!empty($values['date_to'])) {
    $values['date_to'] = date_timestamp_get(new DateObject($values['date_to']));
    $obj->date_to = $values['date_to'];
  }
  else {
    $obj->date_to = 0;
  }
  
  // Save the record.
  drupal_write_record('openadstream_gutters', $obj);

  // Clear the cache.
  _openadstream_clear_gutters_cache();

  drupal_set_message(t('The gutter ad setting has been saved'));
  drupal_goto('admin/build/openadstream/gutters');
}

/**
 * Validate the data submitted in the Gutter Ad form.
 */
function openadstream_admin_gutters_edit_validate($form, &$form_state) {
  $values = $form_state['values'];
  
  // Make sure the From date is not after the Until date.
  if (!empty($values['date_to']) && !empty($values['date_to'])) {
    $date_from = date_timestamp_get(new DateObject($values['date_from']));
    $date_to = date_timestamp_get(new DateObject($values['date_from']));
    
    if ($date_from > $date_to) {
      form_set_error('date_from', t('The "From" date should not be after the "Until" date'));
    }
  }
}

/**
 * Save the data submitted in the Gutter Ad form.
 */
function openadstream_admin_gutters_edit_submit($form, &$form_state) {
  $values = $form_state['values'];

  // Compile the record.
  $obj = new stdClass;
  $obj->id = $values['id'];
  $obj->path = $values['path'];
  $obj->is_include = !!$values['is_include'];
  $obj->bgcolor = $values['bgcolor'];

  // Convert the date field to UNIX timestamp.
  if (!empty($values['date_from'])) {
    $values['date_from'] = date_timestamp_get(new DateObject($values['date_from']));
    $obj->date_from = $values['date_from'];
  }
  else {
    $obj->date_from = 0;
  }

  // Convert the date field to UNIX timestamp.
  if (!empty($values['date_to'])) {
    $values['date_to'] = date_timestamp_get(new DateObject($values['date_to']));
    $obj->date_to = $values['date_to'];
  }
  else {
    $obj->date_to = 0;
  }
  
  // Save the record.
  drupal_write_record('openadstream_gutters', $obj, array('id'));

  // Clear the cache.
  _openadstream_clear_gutters_cache();

  drupal_set_message(t('The gutter ad setting has been saved'));
  drupal_goto('admin/build/openadstream/gutters');
}

/**
 * Verify the user wants to delete a gutter ad setting.
 */
function openadstream_admin_gutters_delete($form, &$form_state, $gutter_id) {
  $gutter = db_query("SELECT g.id, g.path, g.date_from, g.date_to, g.is_include FROM {openadstream_gutters} g WHERE g.id = :gid", array(':gid' => $gutter_id))->fetchAssoc();
  
  drupal_set_title(t("Gutter Ad"), PASS_THROUGH);
  
  $form = array(
    'id' => array(
      '#type' => 'value',
      '#value' => $gutter['id'],
    ),
  );
  
  return confirm_form($form,
    t("Are you sure you want to delete this gutter ad?"),
    'admin/build/openadstream/gutters',
    t('This action cannot be undone'),
    t('Delete'), t('Cancel')
  );
}

/**
 * Process the delete-gutter-ad form.
 */
function openadstream_admin_gutters_delete_submit($formid, &$form_state) {
  if ($form_state['values']['confirm']) {
    db_delete('openadstream_gutters')
      ->condition('id', $form_state['values']['id'])
      ->execute();
    _openadstream_clear_gutters_cache();
    drupal_set_message(t('The gutter has been deleted.'));
  }
  $form_state['redirect'] = 'admin/build/openadstream/gutters';
}

/**
 * Positions setting page.
 */
function openadstream_admin_positions() {
  $output = '';
  
  // Custom positions.
  $positions = _openadstream_admin_positions();
  
  $header = array(
    t('Position Name'), 
    t('Description'), 
    t('Operations'),
  );
  
  $rows = array();
  foreach ($positions as $position) {
    $rows[] = array(
      $position['name'],
      $position['description'],
      l('delete', 'admin/build/openadstream/positions-delete/'. $position['position_id']),
    );
  }
  $output .= "<h3>" . t('Existing Positions') . "</h3>\n";
  $output .= theme('table', array('header' => $header, 'rows' => $rows));
  $output .= "<br />\n";

  // Default positions.
  $positions = _openadstream_invoke_default_positions();

  $header = array(
    t('Position Name'), 
    t('Description'), 
    t('Module'),
  );
  
  $rows = array();
  foreach ($positions as $position) {
    $rows[] = array(
      $position['name'],
      $position['description'],
      $position['module'],
    );
  }
  
  $output .= "<h3>" .  t('Default Positions') . "</h3>\n";
  $output .= theme('table', array('header' => $header, 'rows' => $rows));

  return $output;
}

function openadstream_admin_positions_add() {
  $form = array();
  
  $form['position'] = array(
    '#type' => 'fieldset',
    '#title' => t('Position Settings'),
    '#collapsible' => FALSE,
    '#collapsed' => FALSE,
  );
  
  $form['position']['position_name'] = array(
    '#type' => 'textfield',
    '#title' => t('Position Name'),
    '#description' => t('The position name is set in the OAS management interface.  24/7 RealMedia will provide this value.'),
    '#size' => 30,
    '#maxlength' => 75,
    '#required' => TRUE,
  );

  $form['position']['position_description'] = array(
    '#type' => 'textfield',
    '#title' => t('Description'),
    '#description' => t('An optional description to help remember the details of the advertisement position.'),
    '#size' => 30,
    '#maxlength' => 100,
    '#required' => FALSE,
  );
    
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => 'Save',
  );
  
  return $form;
}

function openadstream_admin_positions_add_validate($form, &$form_state) {
  $values = $form_state['values'];
  
  if (db_query("SELECT COUNT(*) FROM {openadstream_positions} WHERE name = ':name'", array(':name' => $values['position_name']))->fetchField()) {
    form_set_error('info', t('The position name %name is already in use', array('%name' => $values['position_name'])));
  }
}

function openadstream_admin_positions_add_submit($form, &$form_state) {
  $values = $form_state['values'];
  $id = db_insert('openadstream_positions')
    ->fields(array(
      'name' => $values['position_name'],
      'description' => $values['position_description']
    ))
    ->execute();
  _openadstream_clear_positions_cache();
  drupal_set_message(t('The position %name has been created', array('%name' => $values['position_name'])));
  drupal_goto('admin/build/openadstream/positions');
}

function openadstream_admin_positions_delete($form, &$form_state, $position_id) {
  $position = db_query("SELECT * FROM {openadstream_positions} WHERE position_id = :pid", array(':pid' => $position_id))->fetchAssoc();
  
  drupal_set_title(t("Position '@name'", array('@name' => $position['name'])), PASS_THROUGH);
  
  $form = array(
    'position_id' => array(
      '#type' => 'value',
      '#value' => $position_id,
    ),
  );
  
  return confirm_form($form,
    t("Are you sure you want to delete position '%name'?", array('%name' => $position['name'])),
    'admin/build/openadstream/positions',
    t('This action cannot be undone'),
    t('Delete'), t('Cancel')
  );
}

function openadstream_admin_positions_delete_submit($formid, &$form_state) {
  if ($form_state['values']['confirm']) {
    db_delete('openadstream_positions')
      ->condition('position_id', $form_state['values']['position_id'])
      ->execute();
    _openadstream_clear_positions_cache();
    drupal_set_message(t('The position has been deleted'));
  }
  $form_state['redirect'] = 'admin/build/openadstream/positions';
  _block_rehash();
}

/**
 * Pagenames settings page.
 */
function openadstream_admin_pagenames() {
  // Add a header.
  $output = "<h2>" . t('Pagenames') . "</h2>\n";
  $output .= "<p>" . t("Override the Open AdStream pagename variable based on
    the page path. A pagename with path &lt;front&gt; will replace the pagename
    on the home page. The '*' character can be used as a wildcard. The path
    blog/* matches all pages under blog/.") . "</p>\n";
  $output .= "<p>" . t("Pagenamess are checked by weight from lowest to
    highest. The first entry that matches the current path is used.") . "</p>\n";
  $output .= "<br />\n";

  // Compile the custom pagenames.
  $pagenames = _openadstream_admin_pagenames();

  $header = array(
    t('Pagename'), 
    t('Weight'),
    t('Path'),
    t('Operations'),
  );
  
  $rows = array();
  foreach ($pagenames as $pagename) {
    $edit_link = l('edit', 'admin/build/openadstream/pagenames-edit/'. $pagename['pagename_id']);
    $delete_link = l('delete', 'admin/build/openadstream/pagenames-delete/'. $pagename['pagename_id']);
    
    $rows[] = array(
      $pagename['pagename'],
      $pagename['weight'],
      htmlspecialchars($pagename['path']),
      $edit_link .' | '. $delete_link,
    );
  }

  $output .= "<h3>" . t('Custom Pagenames') . "</h3>\n";
  $output .= theme('table', array('header' => $header, 'rows' => $rows));
  $output .= "<br />\n";

  // Compile the default pagenames.
  $pagenames = _openadstream_invoke_default_pagenames();
  $header = array(
    t('Pagename'), 
    t('Weight'),
    t('Path'),
    t('Module'),
  );

  $rows = array();
  foreach ($pagenames as $pagename) {
    $rows[] = array(
      $pagename['pagename'],
      $pagename['weight'],
      htmlspecialchars($pagename['path']),
      $pagename['module'],
    );
  }
  
  $output .= "<h3>" . t('Default Pagenames') . "</h3>\n";
  $output .= theme('table', array('header' => $header, 'rows' => $rows));
  return $output;
}

function openadstream_admin_pagenames_add() {
  $form = array();
  
  $form['pagename'] = array(
    '#type' => 'fieldset',
    '#title' => t('Pagename Settings'),
    '#collapsible' => FALSE,
    '#collapsed' => FALSE,
  );
  
  $form['pagename']['pagename'] = array(
    '#type' => 'textfield',
    '#title' => t('Pagename'),
    '#description' => t('The pagename will be overridden with this value when viewing a page with the specified path.'),
    '#size' => 30,
    '#maxlength' => 75,
    '#required' => TRUE,
  );
  
  $form['pagename']['weight'] = array(
    '#type' => 'textfield',
    '#title' => t('Weight'),
    '#description' => t('Pagenames are applied by order of weight. If a path matches multiple pagename entries then the entry with the highest weight wins.'),
    '#default_value' => 0,
    '#size' => 5,
    '#maxlength' => 5,
    '#required' => TRUE,
  );
  
  $form['pagename']['path'] = array(
    '#type' => 'textfield',
    '#title' => t('Path'),
    '#description' => t("Enter a drupal path. The '*' character is a wildcard. Example paths are blog for the blog page and blog/* for every personal blog. &lt;front&gt; is the front page."),
    '#size' => 30,
    '#maxlength' => 255,
    '#required' => TRUE,
  );
    
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => 'Save',
  );
  
  return $form;
}

function openadstream_admin_pagenames_edit($form, &$form_state, $pagename_id) {
  $pagename = db_query("SELECT * FROM {openadstream_pagenames} WHERE pagename_id = :pid", array(':pid' => $pagename_id))->fetchAssoc();
  
  $form = array();
  
  $form['pagename'] = array(
    '#type' => 'fieldset',
    '#title' => t('Pagename Settings'),
    '#collapsible' => FALSE,
    '#collapsed' => FALSE,
  );
  
  $form['pagename']['pagename'] = array(
    '#type' => 'textfield',
    '#title' => t('Pagename'),
    '#description' => t('The pagename will be overridden with this value when viewing a page with the specified path.'),
    '#size' => 30,
    '#maxlength' => 75,
    '#required' => TRUE,
    '#default_value' => $pagename['pagename'],
  );
  
  $form['pagename']['weight'] = array(
    '#type' => 'textfield',
    '#title' => t('Weight'),
    '#description' => t('Pagenames are applied by order of weight. If a path matches multiple pagename entries then the entry with the highest weight wins.'),
    '#default_value' => $pagename['weight'],
    '#size' => 5,
    '#maxlength' => 5,
    '#required' => TRUE,
  );
  
  $form['pagename']['path'] = array(
    '#type' => 'textfield',
    '#title' => t('Path'),
    '#description' => t("Enter a drupal path. The '*' character is a wildcard. Example paths are blog for the blog page and blog/* for every personal blog. &lt;front&gt; is the front page."),
    '#size' => 30,
    '#maxlength' => 255,
    '#required' => TRUE,
    '#default_value' => $pagename['path'],
  );
  
  $form['pagename']['pagename_id'] = array(
    '#type' => 'value',
    '#value' => $pagename['pagename_id'],
  );
  
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => 'Save',
  );
  
  return $form;
}

function openadstream_admin_pagenames_edit_submit($form, &$form_state) {
  $values = $form_state['values'];
  db_update('openadstream_pagenames')
    ->fields(array(
      'pagename' => $values['pagename'],
      'weight' => $values['weight'],
      'path' => $values['path']
    ))
    ->condition('pagename_id', $values['pagename_id'])
    ->execute();
  _openadstream_clear_pagenames_cache();
  drupal_set_message('Pagename pagename has been updated');
  drupal_goto('admin/build/openadstream/pagenames');
}

function openadstream_admin_pagenames_add_submit($form, &$form_state) {
  $values = $form_state['values'];
  $id = db_insert('openadstream_pagenames')
    ->fields(array(
      'pagename' => $values['pagename'],
      'weight' => $values['weight'],
      'path' => $values['path']
    ))
    ->execute();
  _openadstream_clear_pagenames_cache();
  drupal_set_message(t('The pagename pagename %name has been created', array('%name' => $values['pagename'])));
  drupal_goto('admin/build/openadstream/pagenames');
}

function openadstream_admin_pagenames_delete($form, &$form_state, $pagename_id) {
  $pagename = db_query("SELECT * FROM {openadstream_pagenames} WHERE pagename_id = :pid", array(':pid' => $pagename_id))->fetchAssoc();
  
  drupal_set_title(t("Pagename '@name'", array('@name' => $pagename['pagename'])), PASS_THROUGH);
  
  $form = array(
    'pagename_id' => array(
      '#type' => 'value',
      '#value' => $pagename_id,
    ),
  );
  
  return confirm_form($form,
    t("Are you sure you want to delete pagename '%name'?", array('%name' => $pagename['pagename'])),
    'admin/build/openadstream/pagenames',
    t('This action cannot be undone'),
    t('Delete'), t('Cancel')
  );
}

function openadstream_admin_pagenames_delete_submit($formid, &$form_state) {
  if ($form_state['values']['confirm']) {
    db_delete('openadstream_pagenames')
      ->condition('pagename_id', $form_state['values']['pagename_id'])
      ->execute();
    _openadstream_clear_pagenames_cache();
    drupal_set_message(t('The pagename has been deleted'));
  }
  $form_state['redirect'] = 'admin/build/openadstream/pagenames';
}
