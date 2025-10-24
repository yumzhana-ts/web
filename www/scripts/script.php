<?php
// script.php


$raw = file_get_contents('php://input');
error_log("RAW INPUT: [$raw]");

// Определяем тип данных
$contentType = isset($_SERVER["CONTENT_TYPE"]) ? trim($_SERVER["CONTENT_TYPE"]) : '';

// Массив для хранения распаршенных данных
$data = [];

// Разбираем тело в зависимости от типа
if (strpos($contentType, 'application/json') !== false) {
    $data = json_decode($raw, true); // JSON в массив
} elseif (strpos($contentType, 'application/x-www-form-urlencoded') !== false) {
    parse_str($raw, $data); // form-urlencoded в массив
}

// Если массив пустой, пробуем $_POST как запасной вариант
if (empty($data)) {
    $data = $_POST;
}

// Присваиваем переменные с дефолтами
$name = isset($data['name']) ? $data['name'] : 'Unknown';
$age  = isset($data['age'])  ? $data['age']  : 'Unknown';

// Выводим результат
echo "Hello, $name! You are $age years old.";

// Можно и весь массив вывести для отладки
// print_r($data);
?>
