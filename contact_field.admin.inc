<?php

/**
 * @file
 *
 * Create administer interface to add fields to contact form
 *
 * @author Bhavin H. Joshi <bhavinjosi@joshics.in>
 */


/**
 *
 * @param $ss_type
 * @return unknown_type
 * @author Bhavin H. Joshi <bhavinjosi@joshics.in>
 */
function contact_field_add_field($op, $om_field_details = NULL) {
  $form_state = array(); // because it's local variable, we should initialize it there
  if ($om_field_details) {
    $om_field_details = unserialize($om_field_details);
  }
  $field_type = _get_field_type($op);

  if ($op != 'add') {
    switch ($field_type) {
      case 'textfield':
        if (module_exists('contact_textfield')) {
          return drupal_get_form('contact_field_add_field_text_form', $form_state, $op);
        }
        break;
      case 'textarea':
        if (module_exists('contact_textfield')) {
          return drupal_get_form('contact_field_add_field_textarea_form', $form_state, $op);
        }
        break;
      case 'radios':
      case 'checkboxes':
        if (module_exists('contact_option')) {
          return drupal_get_form('contact_field_add_field_option_form', $form_state, $op);
        }
        break;
      case 'select':
        if (module_exists('contact_listfield')) {
          return drupal_get_form('contact_field_add_field_listfield_form', $form_state, $op);
        }
        break;
      case 'fieldset':
        if (module_exists('contact_listfield')) {
          return drupal_get_form(contact_group_submit($form_state, $op));
        }
        break;
    }
  }
  else {
    switch ($om_field_details->type) {
      case 'text':
        if (module_exists('contact_textfield')) {
          return drupal_get_form('contact_field_add_field_text_form', $form_state, $om_field_details);
        }
        break;
      case 'area':
        if (module_exists('contact_textfield')) {
          return drupal_get_form('contact_field_add_field_textarea_form', $form_state, $om_field_details);
        }
        break;
      case 'option':
        if (module_exists('contact_option')) {
          return drupal_get_form('contact_field_add_field_option_form', $form_state, $om_field_details);
        }
        break;
      case 'list':
        if (module_exists('contact_listfield')) {
          return drupal_get_form('contact_field_add_field_listfield_form', $form_state, $om_field_details);
        }

      case 'fieldset':
        if (module_exists('contact_listfield')) {
          contact_group_submit($form_state, $om_field_details);
        }
        break;
    }
  }
}

/**
 * contact_field_delete_field
 *
 * @param string $field_name
 *  The field name to be deleted
 * @author Bhavin H. Joshi <bhavinjosi@joshics.in>
 */
function contact_field_delete_field($field_name) {
  if ($field_name) {
    db_query("DELETE FROM {contact_fields} WHERE field_name = '%s'", $field_name);
    drupal_goto("admin/build/contact/manage");
  }

}

/**
 *
 * @param string $field
 */
function contact_field_edit_field($field) {
  if ($field) {
    module_load_include('inc', 'contact_field', 'contact_field.admin');
    $field_type = db_query("SELECT field_type FROM {contact_fields} WHERE field_name = ':field'", array(':field' => $field))->fetchField();
    switch($field_type) {
      case 'textfield':
        return drupal_get_form('contact_field_add_field_text_form', $form_state, $field, TRUE);
    }
  }
}

/**
 *
 * @return unknown_type
 * @author Bhavin H. Joshi <bhavinjosi@joshics.in>
 */
function contact_field_list_field() {
  $form = array();
  drupal_set_title(t("Manage fields"));
  $form['#method'] = 'POST';
  $form['contact_field_list'] = array(
    '#type' => 'item',
    '#markup' => _get_fields(),
    '#description' => t("Only fields from Contact form field module can be edited/deleted here."),
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#submit' => array('contact_field_list_update'),
    '#value' => t("Save configuration"),
    '#weight' => 50,
  );

  $form['#validate'] = array('contact_field_validate');
  $form['#submit'] = array('contact_field_list_update');
  return $form;
}


/**
 * contact_field_list_update
 *
 * @param $form
 * @param $form_values
 * @author Bhavin H. Joshi <bhavinjosi@joshics.in>
 */
function contact_field_list_update($form, $form_values) {
  $field_label = $form_values['clicked_button']['#post']['field_label'];
  $field_name = $form_values['clicked_button']['#post']['field_name'];
  $field_type = $form_values['clicked_button']['#post']['field_type'];

  if (module_exists('contact_group')) {
    $ss_group_label = $form_values['clicked_button']['#post']['field_group_label'];
    $ss_group_name = $form_values['clicked_button']['#post']['field_group_name'];
  }
  else {
    $ss_group_label = $ss_group_name = NULL; //just initialize it to avoid Notices.
  }

  if ($field_name && $field_label && $field_type) {
    $om_field_details = (object) array('name' => "field_". $field_name, 'title' => $field_label, 'type' => $field_type);
    drupal_goto("admin/build/contact/add/". serialize($om_field_details));
  }

  elseif ($ss_group_label && $ss_group_name) {
    $om_field_details = (object) array('name' => "group_". $ss_group_name, 'title' => $ss_group_label, 'type' => 'fieldset');
    drupal_goto("admin/build/contact/add/". serialize($om_field_details));
  }

  else {
    $am_weight = preg_grep("/^(contact_weight_)/", array_keys($form_values['clicked_button']['#post']));
    $am_group = preg_grep("/^(contact_group_)/", array_keys($form_values['clicked_button']['#post']));

    if (!empty($am_weight)) {
      foreach ($am_weight as $key => $value) {
        $am_field = preg_split("/^(contact_weight_)/", $value, -1, PREG_SPLIT_NO_EMPTY);
        if (!empty($am_field)) {
          db_query("UPDATE {contact_fields} SET weight = %d WHERE field_name = '%s'",
          $form_values['clicked_button']['#post'][$value], $am_field[0]);
        }
      }
    }

    if (!empty($am_group)) {
      foreach ($am_group as $key => $value) {
        $am_field = preg_split("/^(contact_group_)/", $value, -1, PREG_SPLIT_NO_EMPTY);
        if (!empty($am_field)) {
          db_query("UPDATE {contact_fields} SET field_group = '%s' WHERE field_name = '%s'",
            $form_values['clicked_button']['#post'][$value], $am_field[0]);
        }
      }
    }
  }
}



/**
 * _get_fields
 *
 * Return themed fields
 *
 * @return unknown_type
 * @author Bhavin H. Joshi <bhavinjosi@joshics.in>
 */
function _get_fields() {
  $form_state = array(); // because it's local variable, we should initialize it there
  _set_core_fields();

  $form_id = 'contact_field_list_field';
  $form_values = '';
  $form['#method'] = 'POST';
  $form_state = array();
  $form_state['method'] = 'post';
  $form_state['programmed'] = TRUE;
  $am_weight = array();
  $am_group = array();
  $ss_build_id = 'form-' . drupal_hash_base64(uniqid(mt_rand(), TRUE) . mt_rand());

  $header = array(t("Title"), t("Name"), t("Type"), t("Required"),
    t("Enabled"), t("Weight"), array('data' => t("Operation"), 'colspan' => 2));
  $r_result = db_select('contact_fields', 'cf')
    ->fields('cf')
    ->orderby('weight', 'ASC')
    ->execute();
  $class = "";

  while($om_result = $r_result->fetchObject()) {
    if ($om_result->core) {
      $ss_module = '';
      $class = "menu-disabled";
    }
    else {
      $ss_module = '(Module: Contact form field)';
      $class = '';
    }

    $am_settings = unserialize($om_result->settings);

    $form_part_weight = array(
      '#post' => array(), // otherwise we will have Notice: Undefined index: #post in form_builder() (line 948 of includes\form.inc)
      '#programmed' => TRUE, // otherwise we will have Notice: Undefined index: #programmed in form_builder() (line 949 of includes\form.inc)
      '#tree' => FALSE, // otherwise we will have Notice: Undefined index: #tree in form_builder() (line 952 of includes\form.inc).
    );

    $form_part_weight['contact_weight_'. $om_result->field_name] = array(
      '#type' => 'textfield',
      '#title' => t("Weight"),
      '#size' => 5,
      '#default_value' => $om_result->weight,
      '#attributes' => array('class' => array('contact-weight')),
    );

    if ($om_result->field_type == 'fieldset') {
      $row = array(
        $am_settings['title'],
        drupal_render(form_builder(NULL, $form_part_weight, $form_state)),
        $om_result->field_name,
        array('data' => t("Standard group")),
        NULL,
        NULL,
        drupal_render(drupal_prepare_form($form_id, $am_weight, $form_state)),
        ($om_result->core ? NULL : l(t("Configure"), 'admin/build/contact/'. $om_result->field_name ."/edit")),
        ($om_result->core ? NULL : l(t("Remove"), 'admin/build/contact/'. $om_result->field_name ."/delete")),
      );
      drupal_add_tabledrag('contact-field', 'depth', 'parent', 'contact-group', NULL, 'contact-group-'. $om_result->field_name, TRUE);
      drupal_add_tabledrag('contact-field', 'order', 'sibling', 'contact-weight', NULL, 'contact-weight-'. $om_result->field_name, TRUE);
    }
    else {
      if (module_exists('contact_group')) {

        $form_part_group = array(
          '#post' => array(), // otherwise we will have Notice: Undefined index: #post in form_builder() (line 948 of includes\form.inc)
          '#programmed' => TRUE, // otherwise we will have Notice: Undefined index: #programmed in form_builder() (line 949 of includes\form.inc)
          '#tree' => FALSE, // otherwise we will have Notice: Undefined index: #tree in form_builder() (line 952 of includes\form.inc).
        );

        $form_part_group['contact_group_'. $om_result->field_name ."_parent"] = array(
          '#type' => 'select',
          '#title' => t("Group"),
          '#multiple' => FALSE,
          '#parent' => array("parent"),
          '#options' => _get_contact_group(),
          '#default_value' => $om_result->field_group ? $om_result->field_group : "",
          '#attributes' => array('class' => array('contact-group')),
        );

        $form_part_group[$om_result->field_name] = array(
          '#type' => 'hidden',
          '#parent' => array($om_result->field_name),
          '#default_value' => $om_result->field_name,
        );
      }
      if (module_exists('contact_group') && $om_result->field_group != "") {
        $am_in_group =  array(
          '<div class="indentation">&nbsp;</div>',
          $am_settings['title'] ." $ss_module",
        );
      }
      else {
        $am_in_group =  array(
          $am_settings['title'] ." $ss_module",
        );
      }
      $ss_group_fields = count($am_in_group == 2) ? implode("", $am_in_group) : $am_in_group[0];

      $form_part_weight = form_builder(NULL, $form_part_weight, $form_state);

      if (module_exists('contact_group')) {
        $form_part_group = form_builder(NULL, $form_part_group, $form_state);
        $rendered = drupal_render($form_part_weight) . drupal_render($form_part_group);
      }
      else {
        $rendered = drupal_render($form_part_weight);
      }
      $row = array(
        $ss_group_fields,
        $rendered,
        $om_result->field_name,
        $om_result->field_type,
        ($om_result->required ? t("Yes") : t("No")),
        ($om_result->core ? t("Yes") : ($om_result->enabled ? t("Yes") : t("No"))),
        drupal_render(drupal_prepare_form($form_id, $am_weight, $form_state)),
        ($om_result->core ? NULL : l(t("Configure"), 'admin/build/contact/'. $om_result->field_name ."/edit")),
        ($om_result->core ? NULL : l(t("Remove"), 'admin/build/contact/'. $om_result->field_name ."/delete")),
      );
    }

    $rows[] = array(
      'id' => 'contact-field',
      'data' => $row,
      'class' => 'draggable ' . $class,
    );

    unset($class, $form_part_weight, $form_part_group);
  }

  //Add fields
  $am_fields = module_invoke_all('info');
  $am_field_type[0] = "-- ". t("Field type") ." --";
  if (!empty($am_fields)) {
    foreach ($am_fields as $key => $value) {
      $am_field_type[$value['type']] = $value['title'];
    }
  }
  else {
    drupal_set_message(t("No contact form field module is enabled. Please enable from @module",
      array('!module' => l(t("module page"), 'admin/build/modules'))), 'error');
  }

  $form_label = array(
    '#post' => array(), // otherwise we will have Notice: Undefined index: #post in form_builder() (line 948 of includes\form.inc)
    '#programmed' => TRUE, // otherwise we will have Notice: Undefined index: #programmed in form_builder() (line 949 of includes\form.inc)
    '#tree' => FALSE, // otherwise we will have Notice: Undefined index: #tree in form_builder() (line 952 of includes\form.inc).
  );

  $form_label['field_label'] = array(
    '#type' => 'textfield',
    '#title' => t("New field"),
    '#size' => 30,
    '#description' => t("Label"),
  );

  $form_field_name = array(
    '#post' => array(), // otherwise we will have Notice: Undefined index: #post in form_builder() (line 948 of includes\form.inc)
    '#programmed' => TRUE, // otherwise we will have Notice: Undefined index: #programmed in form_builder() (line 949 of includes\form.inc)
    '#tree' => FALSE, // otherwise we will have Notice: Undefined index: #tree in form_builder() (line 952 of includes\form.inc).
  );

  $form_field_name['field_name'] = array(
    '#type' => 'textfield',
    '#size' => 30,
    '#description' => t("Field name (a-z, 0-9, _)"),
    '#field_prefix' => 'field_'
  );

  $form_field_type = array(
    '#post' => array(), // otherwise we will have Notice: Undefined index: #post in form_builder() (line 948 of includes\form.inc)
    '#programmed' => TRUE, // otherwise we will have Notice: Undefined index: #programmed in form_builder() (line 949 of includes\form.inc)
    '#tree' => FALSE, // otherwise we will have Notice: Undefined index: #tree in form_builder() (line 952 of includes\form.inc).
  );

  $form_field_type['field_type'] = array(
    '#type' => 'select',
    '#description' => t("Set the field type"),
    '#options' => $am_field_type,
  );

  $rows[] = array(
    array('data' => t("Add"), 'colspan' => 8, 'class' => array('region')),
  );

  $form_id = NULL; //just initialize $form_id

  $form_part_label = form_builder($form_id, $form_label, $form_values);
  $form_part_name = form_builder($form_id, $form_field_name, $form_values);
  $form_part_type = form_builder($form_id, $form_field_type, $form_values);

  $rows[] = array(
    drupal_render($form_part_label),
    NULL,
    drupal_render($form_part_name),
    array('data' => drupal_render($form_part_type), 'colspan' => 8),
  );

  unset($form_part_label, $form_part_name, $form_part_type);

  if (module_exists('contact_group')) {
    $form_group_label['#build_id'] = $ss_build_id;
    $form_group_label['field_group_label'] = array(
      '#type' => 'textfield',
      '#title' => t("New group"),
      '#size' => 30,
      '#description' => t("Label"),
    );

    $form_group_name['#build_id'] = $ss_build_id;
    $form_group_name['field_group_name'] = array(
      '#type' => 'textfield',
      '#size' => 30,
      '#description' => t("Group name (a-z, 0-9, _)"),
      '#field_prefix' => 'group_'
    );

    $form_part_group_label = form_builder($form_id, $form_group_label, $form_values);
    $form_part_group_name = form_builder($form_id, $form_group_name, $form_values);

    $rows[] = array(
      'data' => array(
        drupal_render($form_part_group_label),
        array('data' => drupal_render($form_part_group_name), 'colspan' => 7),
      ),
    );

    unset($form_part_group_label, $form_part_group_name);
  }

  drupal_add_tabledrag('contact-field', 'depth', 'parent', 'contact-group', NULL, NULL, TRUE, 1);
  drupal_add_tabledrag('contact-field', 'order', 'sibling', 'contact-weight', NULL, NULL, TRUE);
  $am_variable = array(
    'header' => $header,
    'rows' => $rows,
    'attributes' => array('id' => 'contact-field'),
    'sticky' => TRUE,
  );
  return theme('table', $am_variable);
}




/**
 * _set_core_fields
 *
 * Set the core fields
 *
 * @author Bhavin H. Joshi <bhavinjosi@joshics.in>
 */
function _set_core_fields() {
  $form_id  = 'contact_site_page';
  module_load_include('inc', 'contact', 'contact.pages');
  $form = contact_site_form($form, $form_state);
  $i = -10;
  $am_exclude = array('#token', 'submit', 'copy', 'contact_information', '#attributes', 'actions');
  foreach ($form as $name => $attribute) {
    if (!in_array($name, $am_exclude) && !empty($attribute['#type'])) {
     $result = db_select('contact_fields', 'cf')
        ->fields('cf', array('field_name'))
        ->condition('core', 1, '=')
        ->condition('field_name', $name, '=')
        ->execute()
        ->fetchAssoc();
      if (empty($result['field_name'])) {
        $am_settings = array('title' => $form[$name]['#title']);
        db_insert('contact_fields')
          ->fields(array(
            'field_name' => $name,
            'field_type' => $attribute['#type'],
            'settings' => serialize($am_settings),
            'required' => !empty($attribute['#required']) ? $attribute['#required'] : 0,
            'enabled' => 1,
            'weight' => $i,
            'core' => 1,
            'field_group' => '',
         ))->execute();
        $i++;
      }
    }
  }
}




/**
 * contact_field_list_field
 *
 *  Return field display template
 *
 *  @author Bhavin H. Joshi <bhavinjosi@joshics.in>
 */
function contact_field_display_field() {
  drupal_set_title(t("Message template"));
  $form = array();
  $form['contact_field_use_template'] = array(
    '#type' => 'checkbox',
    '#title' => t("Use message template"),
    '#default_value' => variable_get("contact_field_use_template", 0),
    '#weight' => -10,
  );

  $form['contact_field_message_template'] = array(
    '#type' => 'textarea',
    '#title' => t("Message template"),
    '#default_value' => variable_get("contact_field_message_template", ""),
    '#description' => t("The actual message content will be replaced by the content of message template with fields replaced by actual values."),
    '#weight' => -9,
  );

  $form['contact_field_token'] = array(
    '#type' => 'fieldset',
    '#title' => t("Available fields"),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );

  $form['contact_field_token']['contact_field_lit'] = array(
    '#type' => 'item',
    '#description' => t("You can use above fields in the message template. Please note that some of the fields are of contact module."),
    '#value' => _get_contact_field(),
  );

  drupal_add_tabledrag('contact-field', 'depth', 'parent', 'contact-group');
  drupal_add_tabledrag('contact-field', 'order', 'sibling', 'contact-weight');
  return system_settings_form($form);
}
