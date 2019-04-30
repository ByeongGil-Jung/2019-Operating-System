#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/list.h>
#include <linux/module.h>
#include <linux/slab.h>

struct Birthday {
    int day;
    int month;
    int year;
    struct list_head list;
};

static LIST_HEAD(birthday_list);

struct Birthday *createBirthday(int day, int month, int year)
{
    /* 생일을 위한 메모리를 할당하고, 인자들을 채워 생일을 완성. */
    struct Birthday *birthday;
    birthday = kmalloc(sizeof(*birthday), GFP_KERNEL);

    birthday->day = day;
    birthday->month = month;
    birthday->year = year;

    INIT_LIST_HEAD(&(birthday->list));

    return birthday;
}

int simple_init(void)
{
    printk(KERN_INFO "Loading Module: BDLIST ......\n");

    struct Birthday *cursor;

    /* 생일 목록을 하나씩 생성하는대로 연결리스트에 연결(노드 삽입). */
    struct Birthday *birthday1 = createBirthday(8, 12, 1995);
    struct Birthday *birthday2 = createBirthday(5, 7, 1997);
    struct Birthday *birthday3 = createBirthday(25, 4, 1925);

    list_add_tail(&(birthday1->list), &birthday_list);
    list_add_tail(&(birthday2->list), &birthday_list);
    list_add_tail(&(birthday3->list), &birthday_list);

    /* 완성된 연결리스트를 탐색하는 커널 함수를 사용하여 출력 */
    list_for_each_entry(cursor, &birthday_list, list) {
    	printk(KERN_INFO "OS Module: Day %d.%d.%d\n", cursor->day, cursor->month, cursor->year);
    }

    return 0;
}

void simple_exit(void)
{
    struct list_head *ptr, *next;
    struct Birthday *temp;
    /* 모듈을 제거할 때는 생성한 연결 리스트도 하나씩 제거 */
    if(list_empty(&birthday_list)) {
        printk(KERN_INFO "List is Empty\n");
        return;
    }

    /* 이제 연결리스트를 탐색하면서 하나씩 제거 */
    list_for_each_safe(ptr, next, &birthday_list) {
    	temp = list_entry(ptr, struct Birthday, list);
    	printk(KERN_INFO "OS Module: Removing %d.%d.%d\n", temp->day, temp->month, temp->year);

    	list_del(&(temp->list));
    	kfree(ptr);
    }

    printk(KERN_INFO "Removing Module: BDLIST....\n");
}

module_init(simple_init);
module_exit(simple_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Simple Module");
MODULE_AUTHOR("Byeonggil Jung");
