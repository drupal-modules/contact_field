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
 * @param $ss__type
 * @return unknown_type
 * @author Bhavin H. Joshi <bhavinjosi@joshics.in>
 */
function contact_field_add_field($ss__type) {
  return drupal_get_form('contact_field_add_field_'. $ss__type .'_form');
}



/**
 * 
 * @return unknown_type
 * @author Bhavin H. Joshi <bhavinjosi@joshics.in>
 */
function contact_field_list_field() {
  drupal_set_title(t("Manage fields"));
  $output = _get_fields();
  $links = array();
  $links[] = array(
    'href' => 'admin/build/contact/add/text',
    'title' => t("Text field"),
  );
  $links[] = array(
    'href' => 'admin/build/contact/add/textarea',
    'title' => t("Text area"),
  );
  $links[] = array(
    'href' => 'admin/build/contact/add/option',
    'title' => t("Radio/Check box"),
  );
  $links[] = array(
    'href' => 'admin/build/contact/add/list',
    'title' => t("List"),
  );
  return $output . theme('links', $links, '');
}



/**
 * contact_field_add_field_text_form
 * 
 * Form builder function
 * 
 * @return unknown_type
 * @author Bhavin H. Joshi <bhavinjosi@joshics.in>
 */
function contact_field_add_field_text_form() {
  drupal_set_title(t("Configure contact field"));
  $form = array();
  
  $form['field_type'] = array(
    '#type' => 'hidden',
    '#default_value' => 'textfield',
  );
  
  $form['contact_field_name'] = array(
    '#type' => 'textfield',
    '#title' => t("Field name"),
    '#description' => t("Machine readable name of the field. This should only
      contain alphanumaric charachters and underscore."),
    '#required' => TRUE,
    '#weight' => -50,
  );

  $form['contact_field_label'] = array(
    '#type' => 'textfield',
    '#title' => t("Field title"),
    '#description' => t("Provide the title of the field"),
    '#required' => TRUE,
    '#weight' => -49,
  );

  $form['contact_field_require'] = array(
    '#type' => 'radios',
    '#title' => t("Required?"),
    '#description' => t("Whether this field is mandatory."),
    '#options' => array(
      0 => t("No"),
      1 => t("Yes"),
    ),
    '#required' => TRUE,
    '#weight' => -48,
  );

  $form['contact_field_enabled'] = array(
    '#type' => 'checkbox',
    '#title' => t("Display this field"),
    '#description' => t("Check this box to make this field available on 
      contact form."),
    '#weight' => -47,
  );

  $form['#submit'] = array('contact_field_submit');
  $form['contact_field_submit'] = array(
    '#type' => 'submit',
    '#value' => t("Save configuration"),
  );

  return $form;
}




/**
 * Implementation of hook_submit
 * 
 * @param $form
 * @param $form_values
 * @return unknown_type
 * @author Bhavin H. Joshi <bhavinjosi@joshics.in>
 */
function contact_field_submit($form, $form_values) {
  $ss__field_name = $form_values['values']['contact_field_name'];
  $ss__field_type = $form_values['values']['field_type'];
  $ss__field_required = $form_values['values']['contact_field_require'];
  $ss__field_enabled = $form_values['values']['contact_field_enabled'];
  $ss__field_settings['title'] = $form_values['values']['contact_field_label'];

  db_query("INSERT INTO {contact_fields} VALUES('%s', '%s', '%s', %d, %d)",
    $ss__field_name, $ss__field_type, serialize($ss__field_settings),
    $ss__field_required, $ss__field_enabled);
  drupal_set_message($ss__field_settings['title'] ." ". t("created"));
  drupal_goto('admin/build/contact/manage');
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
  $header = array(t("Title"), t("Name"), t("Type"), t("Required"),
    t("Enabled"), NULL, NULL);
  $r__result = db_query("SELECT * FROM {contact_fields}");
  while ($om__result = db_fetch_object($r__result)) {
    $am__settings = unserialize($om__result->settings);
    $row = array(
      $am__settings['title'],
      $om__result->field_name,
      $om__result->field_type,
      $om__result->required,
      $om__result->enabled,
      l(t("edit"), 'admin/build/contact/'. $om__result->field_name ."/edit"),
      l(t("delete"), 'admin/build/contact/'. $om__result->field_name ."/delete"),
    );
    $rows[] = array(
      'data' => $row,
      'class' => 'draggable',
    );
  }
//  drupal_add_tabledrag('contact-field', 'order', 'order', 'sibling');
  return theme('table', $header, $rows, array('id' => 'contact-field'));
}