# Do actions

Единственным аргументов программы является начальное значение счетчика, которое затем будет изменяться.

Программа при запуске сообщает на стандартный поток вывода свой `PID`.

При поступлении сигнала `SIGUSR1` увеличить текущее значение на 1 и вывести его на стандартный поток вывода.

При поступлении сигнала `SIGUSR2` - умножить текущее значение на `-1` и вывести его на стандартный поток вывода.

При поступлении одного из сигналов `SIGTERM` или `SIGINT` необходимо завершить свою работу с кодом возврата `0`.

Семантика повединия сигналов (Sys-V или BSD) считается не определенной.

Не забывайте выталкивать буфер вывода.
