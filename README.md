# Задание 1 (System V)
Реализовать 2 программы, первая сервер, вторая клиент. Сервер создает 
сегмент разделяемой памяти (достаточный для хранения сообщений) и 
записывает сообщение виде строки “Hi!”, ждет ответа от клиента и 
выводит на экран, удаляет сегмент разделяемой памяти. Клиент 
подключается к сегменту разделяемой памяти и считывает сообщение от 
сервера, выводит на экран, отвечает серверу сообщением виде строки 
“Hello!”. Сделать это как для POSIX, так и для SYSTEM V стандартов. 