# Stone for Pebble

A simple Pebble watchface that displays the time, date, day of the week, and weather.

![screenshot](https://github.com/scizzorz/pebble-stone/blob/master/screenshot.png)

## Usage

Get a forecast.io API key [here](https://developer.forecast.io/register). Put the `server/weather.php` file somewhere that can be accessed from your phone. Make a file called `config.php` in the same directory with the following contents:

	<?php
	define('API_KEY', 'your API key here');

Back in your `pebble-stone` directory, make a file called `src/config.h` with the following contents:

	#define SERVER_URL "http://yourwebsite.com/pebble/weather.php"

Refer to the [Pebble SDK documentation](http://developer.getpebble.com/1/GettingStarted/UsingExamples) to configure, build, and install the app.
