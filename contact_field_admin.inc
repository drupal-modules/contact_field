<?php
//$Id$

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
function contact_field_add_field($op, $ss__type = NULL) {
	if ($op != 'add') {
		$ss__field_type = _get_field_type($op);
		switch($ss__field_type) {
	    case 'text':
	      if (module_exists('contact_textfield')) {
	        return drupal_get_form('contact_field_add_field_text_form', $op);
	      }
	      break;
	    case 'area':
	      if (module_exists('contact_textfield')) {
	        return drupal_get_form('contact_field_add_field_textarea_form', $op);
	      }
	      break;
	    case 'radios':
	    case 'checkboxes':
	      if (module_exists('contact_option')) {
	        return drupal_get_form('contact_field_add_field_option_form', $op);
	      }
	      break;
	    case 'select':
	      if (module_exists('contact_listfield')) {
	        return drupal_get_form('contact_field_add_field_listfield_form', $op);
	      }
	      break;
	  }
	}
	else{
		switch($ss__type) {
	    case 'text':
	      if (module_exists('contact_textfield')) {
	        return drupal_get_form('contact_field_add_field_text_form', $form_state);
	      }
	      break;
	    case 'area':
	      if (module_exists('contact_textfield')) {
	        return drupal_get_form('contact_field_add_field_textarea_form', $form_state);
	      }
	      break;
	    case 'option':
	      if (module_exists('contact_option')) {
	        return drupal_get_form('contact_field_add_field_option_form', $form_state);
	      }
	      break;
	    case 'list':
	      if (module_exists('contact_listfield')) {
	        return drupal_get_form('contact_field_add_field_listfield_form', $form_state);
	      }
	      break;
	  }
	}
}


/**
 * 
 * @return unknown_type
 * @author Bhavin H. Joshi <bhavinjosi@joshics.in>
 */
function contact_field_list_field() {
	$am__fields = module_invoke_all('info');
  drupal_set_title(t("Manage fields"));
  $output = _get_fields();
  $links = array();
  foreach ($am__fields as $key => $value) {
  	$links[] = array(
  	  'href' => 'admin/build/contact/add/'. $value['type'],
  	  'title' => t("Add a") ." ". $value['title'],
  	  'description' => $value['description'],
  	);
  }
  return $output . theme('links', $links, '');
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
    t("Enabled"), array('data' => t("Operation"), 'colspan' => 2));
  $r__result = db_query("SELECT * FROM {contact_fields}");
  while ($om__result = db_fetch_object($r__result)) {
    $am__settings = unserialize($om__result->settings);
    $row = array(
      $am__settings['title'],
      $om__result->field_name,
      $om__result->field_type,
      ($om__result->required ? t("Yes") : t("No")),
      ($om__result->enabled ? t("Yes") : t("No")),
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