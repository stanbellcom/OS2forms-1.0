<?php

function os2forms_server_communication_settings_page() {

  $headers = array(
    array('data' => t('ID'), 'field' => 'id'),
    array('data' => t('Frontend server')),
    array('data' => t('URL')),
    array('data' => t(''))
  );
  $query = db_select('os2forms_server_communication_servers_list', 's');
  $query->fields('s', array('id', 'server_name', 'url'));

  $result = $query->execute()
    ->fetchAllAssoc('id');

  $rows = array();

  foreach ($result as $row) {
    $links = '<a href="/admin/config/os2forms_server_communication/server/' . $row->id . '/edit"> Edit </a>&nbsp <a href="/admin/config/os2forms_server_communication/server/' . $row->id . '/delete"> Delete </a>';

    $rows[] = array_merge(array_values((array) $row), array($links));
  }

  $output = "<h3>Frontend servers</h3>";
  $output .='<ul class="action-links"> <li>'
    . l(t('Add'), '/admin/config/os2forms_server_communication/server/add', array('attributes' => array('name' => 'add_server')))
    . '</li></ul>';
  $output .= theme('table', array('header' => $headers, 'rows' => $rows));
  return $output . render(drupal_get_form('os2forms_server_communication_submissions_download_settings_form'));
}

/**
 * Implements hook_FORM_alter().
 */
function os2forms_server_communication_add_server_form($form, $form_state) {

  return os2forms_server_communication_server_form();
}

function os2forms_server_communication_delete_server_form($form, $form_state) {

  if (isset($form_state['input']['op']) && $form_state['input']['op'] == t('Cancel')) {
    drupal_goto('/admin/config/os2forms_server_communication/settings');
  }
  $server_id = arg(4);
  $query = db_select('os2forms_server_communication_servers_list', 's');
  $query->fields('s', array('server_name'));
  $query->condition('s.id', $server_id, '=');
  $result = $query->execute()
    ->fetchField();

  $form['confirmation_text'] = array(
    '#markup' => '<h3>Are you sure you want to delete <strong>' . $result . '</strong> ?</h3> <p>This action cannot be undone.</p>'
  );
  $form['id'] = array(
    '#type' => 'hidden',
    '#default_value' => $server_id
  );
  $form['delete'] = array(
    '#type' => 'submit',
    '#value' => t('Delete')
  );
  $form['cancel'] = array(
    '#type' => 'button',
    '#value' => t('Cancel')
  );
  return $form;
}

function os2forms_server_communication_delete_server_form_submit($form, $form_state) {
  $input = $form_state['input'];
  extract($input);
  db_delete('os2forms_backend_servers_list')
    ->condition('id', $id)
    ->execute();
  drupal_set_message('Server deleted', 'status');
  drupal_goto('/admin/config/os2forms_server_communication/settings');
}

function os2forms_server_communication_edit_server_form($form, $form_state, $server_id = null) {

  $server_id = arg(4);
  $query = db_select('os2forms_server_communication_servers_list', 's');
  $query->fields('s', array('id', 'server_name', 'url', 'username', 'psw'));
  $query->condition('s.id', $server_id, '=');
  $result = $query->execute()
    ->fetchObject();
  $form = os2forms_server_communication_server_form();
  $form["server_name"]['#default_value'] = $result->server_name;
  $form["server_url"]['#default_value'] = $result->url;
  $form["username"]['#default_value'] = $result->username;
  $form["password"]['#default_value'] = decrypt($result->psw);
  $form["id"]['#default_value'] = $result->id;
  return $form;
}

function os2forms_server_communication_server_form() {

  $form['server_name'] = array(
    '#type' => 'textfield',
    '#title' => t('Server name:'),
    '#size' => 60,
    '#maxlength' => 128,
    '#required' => TRUE,
    '#description' => t('Name of fronend server'),
  );
  $form['id'] = array(
    '#type' => 'hidden',
  );

  $form['server_url'] = array(
    '#type' => 'textfield',
    '#title' => t('URL:'),
    '#size' => 60,
    '#maxlength' => 128,
    '#required' => TRUE,
    '#description' => t('URL for server access'),
  );
  $form['username'] = array(
    '#type' => 'textfield',
    '#title' => t('User name:'),
    '#size' => 60,
    '#maxlength' => 128,
    '#required' => TRUE,
    '#description' => t('User authentificated on frontend server'),
  );
  $form['password'] = array(
    '#type' => 'textfield',
    '#title' => t('Password:'),
    '#size' => 60,
    '#maxlength' => 128,
    '#required' => TRUE,
    '#description' => t('Passwod for frontend server access'),
  );

  $form['add'] = array(
    '#type' => 'submit',
    '#value' => t('Save')
  );
  return $form;
}

function os2forms_server_communication_add_server_form_submit($form, $form_state) {
  $input = $form_state['input'];
  extract($input);

  db_insert('os2forms_server_communication_servers_list')
    ->fields(array(
      'server_name' => $server_name,
      'url' => $server_url,
      'username' => $username,
      'psw' => encrypt($password),
    ))
    ->execute();

  drupal_set_message('Server addad', 'status');
  drupal_goto('/admin/config/os2forms_server_communication/settings');
}

function os2forms_server_communication_edit_server_form_submit($form, $form_state) {
  $input = $form_state['input'];
  extract($input);

  db_update('os2forms_server_communication_servers_list')
    ->fields(array(
      'server_name' => $server_name,
      'url' => $server_url,
      'username' => $username,
      'psw' => encrypt($password),
    ))
    ->condition('id', $id, '=')
    ->execute();

  drupal_set_message('Server added', 'status');
  drupal_goto('/admin/config/os2forms_server_communication/settings');
}

function os2forms_server_communication_submissions_download_settings_form($form, $form_state) {
   $form['os2forms_server_communication_submissions_download'] = array(
        '#type' => 'fieldset',
        '#collapsible' => TRUE,
        '#collapsed' => FALSE,
        '#title' => t('Webform submissions save path'),
    );
    $form['os2forms_server_communication_submissions_download']['os2forms_server_communication_submission_pdf_dir'] = array(
        '#type' => 'textfield',
        '#title' => t('Path to dir :'),
        '#maxlength' => 200,
        '#description' => t('This is the path where saved submissions pdf files'),
        '#default_value' => variable_get('os2forms_server_communication_submission_pdf_dir', 'public://'),
        '#element_validate' => array('_os2forms_server_communication_submission_pdf_dir_validate'),
    );
    

    return system_settings_form($form);
}

function _os2forms_server_communication_submission_pdf_dir_validate($element, &$form_state) {
    $value = $element['#value'];
    if (!file_exists(drupal_realpath($value))) {

        form_error($element, "Directory " . $value . " does nor exist");
    }
    if (!is_writable(drupal_realpath($value))) {

        form_error($element, "Directory " . $value . " is not writable. Check permissions");
    }
}
