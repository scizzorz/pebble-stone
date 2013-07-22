<?php

// config.php defines the API_KEY constant
include('config.php');

$payload = json_decode(file_get_contents('php://input'), true);
if(!$payload) die();
$payload[1] /= 10000;
$payload[2] /= 10000;

$url = "http://api.forecast.io/forecast/" . API_KEY . "/$payload[1],$payload[2]?units=$payload[3]&exclude=hourly,daily,alerts";

$forecast = json_decode(@file_get_contents($url));
if(!$forecast) die();

$response = array();
$icons = array(
    'clear-day' => 0,
    'clear-night' => 1,
    'rain' => 2,
    'snow' => 3,
    'sleet' => 4,
    'wind' => 5,
    'fog' => 6,
    'cloudy' => 7,
    'partly-cloudy-day' => 8,
    'partly-cloudy-night' => 9
);

$icon_id = $icons[$forecast->currently->icon];
$temp = round($forecast->currently->temperature);

if($temp < 0) {
    $temp = -$temp;
    $temp = $temp | (1 << 10);
}
$temp = $temp | ($icon_id << 11);

$response[1] = array('S', $temp);
print json_encode($response);
