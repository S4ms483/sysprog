#include <stdio.h>
#include <pthread.h>
//-lpthread или -pthread  для успешной компиляции добавтить одну из этих опций
//Добавить вызов any_func в основную нить выполнения, чтобы распечатать 2009
void *any_func(void *arg)
{
    // Воспринимаем указатель void* как int* и разыменовываем его так как теперь мы знаем
    // как декодировать биты которые лжат по этому адресу и сколько их нужно
    int a = *(int *)arg;
    a++;
    return (void *)(long)a; //возвращаем а+1 если его воспринимать как void*
}

int main(void)
{
    pthread_t thread; // создаем поток
    int parg = 2007, pdata; // parg - аргумент функции потока, 
                            // pdata - переменная для будущего результата

    if (pthread_create(&thread, NULL, &any_func, &parg) != 0) // создаем еще один поток исполнения
    {
        fprintf(stderr, "Error\n");
        return 1;
    }

    void *result;
    pthread_join(thread, &result); // останавливаем исполнение основного потока пока не выполнится 
                                   // дополнительный поток который мы до этого создали
                                   // result получит значение которое вернет функция потока
    pdata = (int)(long)result;
    printf("%d\n", pdata);         // печатаем результат доп потока который уже один раз вызвал any_func

    // Вызываем any_func в основном потоке
    pdata = (int)(long)any_func(&pdata); // вызываем ее еще раз то есть еще раз увеличиваем на 1
    printf("%d\n", pdata);

    return 0;
}