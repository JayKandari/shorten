<?php

namespace Drupal\shorten\Form;

use Drupal\Core\Form\FormBase;
use Symfony\Component\HttpFoundation\Request;
use Drupal\Core\Form\FormStateInterface;

/**
 * Builds a form which allows shortening of a URL via the UI.
 */
class ShortenShortenForm extends FormBase {

  /**
   * {@inheritdoc}
   */
  public function getFormId() {
    return 'shorten_form_shorten';
  }

  /**
   * {@inheritdoc}
   */
  public function buildForm(array $form, FormStateInterface $form_state, Request $request = NULL) {
    $form_state_values = $form_state->getValues();
    // @FIXME
  // The Assets API has totally changed. CSS, JavaScript, and libraries are now
  // attached directly to render arrays using the #attached property.
  //
  //
  // @see https://www.drupal.org/node/2169605
  // @see https://www.drupal.org/node/2408597
  // drupal_add_js(drupal_get_path('module', 'shorten') . '/shorten.js');

    //Form elements between ['opendiv'] and ['closediv'] will be refreshed via AHAH on form submission.
    $form['opendiv'] = array(
      '#markup' => '<div id="shorten_replace">',
    );
    if (!isset($form_state_values['storage'])) {
      $form_state_values['storage'] = array('step' => 0);
    }
    if (isset($form_state_values['storage']['short_url'])) {
      // This whole "step" business keeps the form element from being cached.
      $form['shortened_url_' . $form_state_values['storage']['step']] = array(
        '#type' => 'textfield',
        '#title' => t('Shortened URL'),
        '#default_value' => $form_state_values['storage']['short_url'],
        '#size' => 25,
        '#attributes' => array('class' => array('shorten-shortened-url')),
      );
    }
    $form['url_' . $form_state_values['storage']['step']] = array(
      '#type' => 'textfield',
      '#title' => t('URL'),
      '#default_value' => '',
      '#required' => TRUE,
      '#size' => 25,
      '#maxlength' => 2048,
      '#attributes' => array('class' => array('shorten-long-url')),
    );
    //Form elements between ['opendiv'] and ['closediv'] will be refreshed via AHAH on form submission.
    $form['closediv'] = array(
      '#markup' => '</div>',
    );
    $last_service = NULL;
    if (isset($form_state_values['storage']['service'])) {
      $last_service = $form_state_values['storage']['service'];
    }
    $service = _shorten_service_form($last_service);
    if (is_array($service)) {
      $form['service'] = $service;
    }
    $form['shorten'] = array(
      '#type' => 'submit',
      '#value' => t('Shorten'),
      '#ajax' => array(
        'callback' => 'shorten_save_js',
        'wrapper' => 'shorten_replace',
        'effect' => 'fade',
        'method' => 'replace',
      ),
    );
    return $form;
  }

  /**
   * {@inheritdoc}
   */
  public function submitForm(array &$form, FormStateInterface $form_state) {
    $values = $form_state->getValues();
    $this->config('your_module.settings')
      ->set('your_message', $values['your_message'])
      ->save();
    $service = '';
    if ($values['values']['service']) {
      $service = $values['values']['service'];
    }
    $shortened = shorten_url($values['values']['url_' . $values['storage']['step']], $service);
    if ($values['values']['service']) {
      $_SESSION['shorten_service'] = $values['values']['service'];
    }
    drupal_set_message(t('%original was shortened to %shortened', array('%original' => $values['values']['url_' . $values['storage']['step']], '%shortened' => $shortened)));
    $values['rebuild'] = TRUE;
    if (empty($values['storage'])) {
      $values['storage'] = array();
    }
    $values['storage']['short_url'] = $shortened;
    $values['storage']['service']   = $values['values']['service'];
    if (isset($values['storage']['step'])) {
      $values['storage']['step']++;
    }
    else {
      $values['storage']['step'] = 0;
    }
  }
}
