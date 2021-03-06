<?php

/**
 * @file
 * Widget for facet ranges rendered as text links.
 */

class SearchApiRangesWidgetSelect extends FacetapiWidgetLinks {

  /**
   * Renders the links.
   */
  public function execute() {
    static $count_select_widget;
    $count_select_widget++;

    $element = &$this->build[$this->facet['field alias']];

    // Get Search API stuff.
    $searcher = $this->facet->getAdapter()->getSearcher();
    $index_id = explode('@', $searcher);
    $index = search_api_index_load($index_id[1]);
    list($query, $results) = $this->facet->getAdapter()->getCurrentSearch();

    // Prepare variables for min/max query.
    $variables = array(
      'element' => $element,
      'index' => $index,
      'range_field' => $this->facet['field alias'],
      'target' => $this->facet->getAdapter()->getSearchPath(),
      'query' => $query,
      'prefix' => $this->settings->settings['prefix'],
      'suffix' => $this->settings->settings['suffix'],
    );

    // Generate the ranges to the be used for the text links.
    if (strlen($this->settings->settings['range_advanced']) == 0) {
      $element = search_api_ranges_generate_ranges_simple($variables, $this->settings->settings['range_simple']);
    }
    else {
      $element = search_api_ranges_generate_ranges_advanced($variables, $this->settings->settings['range_advanced']);
    };

    $cp_element = $element;
    $first = array_slice($cp_element, 0, 1);
    $cp_first = $first;
    if (isset($cp_first[0]) && count($cp_first[0]['#query']['f']) > 0) {
      foreach ($cp_first[0]['#query']['f'] as $key => $qstring) {
        $values = explode(':', $qstring);
        if ($values[0] == $this->facet['field alias']) {
          $instring = TRUE;
          $cp_first[0]['#query']['f'][$key] = $this->facet['field alias'] . ':';
        }
        else {
          array_push($cp_first[0]['#query']['f'], $this->facet['field alias'] . ':');
        };
      };
    };
    $tmp_first = array_pop($cp_first);
    $remove_facet_url = url($tmp_first['#path']);
    if ($tmp_first['#active'] == TRUE) {
      $remove_facet_url = url($_GET['q'], array('query' => $tmp_first['#query']));
    };

    $options[$remove_facet_url] = t('All');
    $active = $remove_facet_url;
    foreach ($element as $value => $item) {
      $instring = FALSE;
      if (count($item['#query']['f']) > 0) {
        foreach ($item['#query']['f'] as $qstring) {
          $values = explode(':', $qstring);
          if ($values[0] == $this->facet['field alias']) {
            $instring = TRUE;
          };
        };
      };
      if (!$instring) {
        if (!is_array($item['#query']['f'])) {
          $item['#query']['f'] = array();
        };
        array_push($item['#query']['f'], $this->facet['field alias'] . ':' . $value);
      };

      $url = url($_GET['q'], array('query' => $item['#query']));
      $options[$url] = $item['#markup'] . ' (' . $item['#count'] . ')';
      if ($item['#active'] == TRUE) {
        $active = $url;
      }
    };

    if (end($options) !== '(-)') {
      if (!isset($form_state)) {
        $form_state = array();
      };
      $element = facetapi_ranges_select_facet_form($form_state, $options, $count_select_widget, $active);
    };
  }

  /**
   * Allows the widget to provide additional settings to the form.
   */
  function settingsForm(&$form, &$form_state) {
    $form['widget']['widget_settings']['links'][$this->id]['name'] = array(
      '#type' => 'textfield',
      '#title' => t('Name'),
      '#default_value' => $this->settings->settings['name'],
      '#description' => t('The name of the range field.'),
      '#states' => array(
        'visible' => array(
          'select[name="widget"]' => array('value' => $this->id),
        ),
        'enabled' => array(
          'select[name="widget"]' => array('value' => $this->id),
        ),
      ),
    );
    $form['widget']['widget_settings']['links'][$this->id]['prefix'] = array(
      '#type' => 'textfield',
      '#title' => t('Prefix'),
      '#default_value' => $this->settings->settings['prefix'],
      '#description' => t('Adds a prefix to the text links, e.g. $, #.'),
      '#states' => array(
        'visible' => array(
          'select[name="widget"]' => array('value' => $this->id),
        ),
        'enabled' => array(
          'select[name="widget"]' => array('value' => $this->id),
        ),
      ),
    );
    $form['widget']['widget_settings']['links'][$this->id]['suffix'] = array(
      '#type' => 'textfield',
      '#title' => t('Suffix'),
      '#default_value' => $this->settings->settings['suffix'],
      '#description' => t('Adds a suffix to the text links, e.g. &euro;, pcs., etc.'),
      '#states' => array(
        'visible' => array(
          'select[name="widget"]' => array('value' => $this->id),
        ),
        'enabled' => array(
          'select[name="widget"]' => array('value' => $this->id),
        ),
      ),
    );
    $form['widget']['widget_settings']['links'][$this->id]['range_simple'] = array(
      '#type' => 'textfield',
      '#title' => t('Simple range'),
      '#default_value' => $this->settings->settings['range_simple'],
      '#description' => t('Add a fixed range to create "range facet". 
        For example, "10" on a price field will create 10 - 20, 20 - 30 etc.'),
      '#states' => array(
        'visible' => array(
          'select[name="widget"]' => array('value' => $this->id),
        ),
        'enabled' => array(
          'select[name="widget"]' => array('value' => $this->id),
        ),
      ),
    );
    $form['widget']['widget_settings']['links'][$this->id]['range_advanced'] = array(
      '#type' => 'textarea',
      '#title' => t('Advanced range'),
      '#default_value' => $this->settings->settings['range_advanced'],
      '#description' => t('Add a pre-defined list of ranges to create 
        "range facets". For examples, refer to README in search api range.'),
      '#states' => array(
        'visible' => array(
          'select[name="widget"]' => array('value' => $this->id),
        ),
        'enabled' => array(
          'select[name="widget"]' => array('value' => $this->id),
        ),
      ),
    );
  }

  /**
   * Returns defaults for the settings this widget provides.
   */
  function getDefaultSettings() {
    return array(
      'name' => '',
      'prefix' => '',
      'suffix' => '',
      'range_simple' => 10,
      'range_advanced' => '',
    );
  }

}

function facetapi_ranges_select_facet_form($form_state, $options, $count = 0, $active) {

  $name = 'facetapi_ranges_select_facet_form_' . $count;
  $query_string = $_GET;

  unset($query_string['q']);

  $form['facets'] = array(
    '#type' => 'select',
    '#id' => $name,
    '#options' => $options,
    '#value' => $active,
    '#attributes' => array(
      'onchange' => "top.location.href=document.getElementById('$name').options[document.getElementById('$name').selectedIndex].value"),
  );

  return $form;
}

