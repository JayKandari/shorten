<?php

namespace Drupal\shorten_cs\Form;

use Drupal\Core\Form\ConfigFormBase;
use Symfony\Component\HttpFoundation\Request;
use Drupal\Core\Form\FormStateInterface;

/**
 * Settings form.
 */
class CustomServicesEditForm extends ConfigFormBase {

  /**
   * {@inheritdoc}
   */
  public function getFormId() {
    return 'shorten_cs_edit';
  }

  /**
   * {@inheritdoc}
   */
  protected function getEditableConfigNames() {
    return [
      'shorten.settings',
    ];
  }

  /**
   * {@inheritdoc}
   */
  public function buildForm(array $form, FormStateInterface $form_state, Request $request = NULL) {
    $config = $this->config('shorten.settings');
    // $form = shorten_cs_add_form($form, $form_state);
    $form = \Drupal::formBuilder()->getForm('shorten_cs_edit');
    // return \Drupal::service("renderer")->render($form);

    foreach (array('name', 'url', 'type', 'tag') as $key) {
      $form[$key]['#default_value'] = $service->{$key};
    }
    $form['sid'] = array(
      '#type' => 'value',
      '#value' => $service->sid,
    );
    $form['old_name'] = array(
      '#type' => 'value',
      '#value' => $service->name,
    );
    return parent::buildForm($form, $form_state);
  }

  /**
   * {@inheritdoc}
   */
  public function submitForm(array &$form, FormStateInterface $form_state) {
    $values = $form_state->getValues();
    $record = array();
    foreach (array('name', 'url', 'type', 'tag') as $key) {
      $record[$key] = $values[$key];
    }
    \Drupal::database()->insert('shorten_cs')->fields($record)->execute();
  }

  /**
   * {@inheritdoc}
   */
  public function validateForm(array &$form, FormStateInterface $form_state) {
    $v = $form_state->getValues();
    if (($v['type'] == 'xml' || $v['type'] == 'json') && empty($v['tag'])) {
      $form_state->setErrorByName('type', t('An XML tag or JSON key is required for services with a response type of XML or JSON.'));
    }
    $exists = db_query("SELECT COUNT(sid) FROM {shorten_cs} WHERE name = :name", array(':name' => $v['name']))->fetchField();
    if ($exists > 0) {
      $form_state->setErrorByName('name', t('A service with that name already exists.'));
    }
    else {
      $all_services = \Drupal::moduleHandler()->invokeAll('shorten_service');
      $all_services['none'] = t('None');
      foreach ($all_services as $key => $value) {
        if ($key == $v['name']) {
          $form_state->setErrorByName('name', t('A service with that name already exists.'));
          break;
        }
      }
    }
  }
}
