<?php

/**
 * @file
 * Hooks provided by Open AdStream module.
 */

/**
 * Define ad positions in this function. The positions will appear under
 * 'Default Positions' on admin/settings/openadstream/positions.  The return must
 * be an array of arrays; the sub-arrays must have a field 'name' which is the
 * position name presented by 24/7 Real Media, while an optional 'description'
 * field is optional and can be used to explain what the position's dimensions
 * are or what it is used for.
 */
function hook_openadstream_default_positions() {
  $positions = array(
    array(
      'name' => 'Top',
      'description' => '728x90 banner for the page header',
    ),
    array(
      'name' => 'Right',
      'description' => '300x250, suitable for sidebars',
    ),
  );
  return $positions;
}

/**
 * Define pagename overrides in this function. These overrides will be
 * preempted by any overrides manually defined in the admin gui. You can
 * view all overrides at admin/settings/openadstream/overrides. Overrides 
 * are evaluated from lowest weight to highest.
 */
function hook_openadstream_default_pagenames() {
  $overrides = array(
    array('pagename' => 'node_module', 'weight' => 0, 'path' => 'node/*'),
    array('pagename' => 'toyota', 'weight' => 0, 'path' => 'cars/purple/*'),
  );
  return $overrides;
}

/**
 * Execute custom logic to customize the pagename; the automatically generated
 * pagename is passed as an argument so it may be modified.  The function must
 * return a full string which will replace the passed-in pagename.
 */
function hook_openadstream_override_pagename($pagename) {
  $new_pagename = FALSE;
  // All node pages other than node/*/edit.
  if (arg(0) == 'node' && is_numeric(arg(1)) && arg(2) != 'edit') {
    // Load the node.
    $node = node_load(arg(1));
    // Whatever logic is needed.
    $new_pagename = $node->uid . '-' . preg_replace('/[^a-zA-Z0-9\s]/', '-', strtolower($node->title));
  }
  return $new_pagename;
}

