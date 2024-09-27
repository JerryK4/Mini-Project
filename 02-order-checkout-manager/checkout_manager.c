#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<pthread.h>
#include<fcntl.h>
#include<unistd.h>
#include<errno.h>
#include<semaphore.h>

#define handle_error(msg) \
    do{perror(msg);exit(EXIT_FAILURE);} while(0)

#define FIFO_NAME "./order_fifo"
#define BUFF_SIZE 1024
#define NUM_CHECKOUTS 3 //Số lượng quầy thanh toán 

sem_t checkout_sem[NUM_CHECKOUTS];//Mảng semaphore cho các quầy thanh toán 

char order_buffer[NUM_CHECKOUTS][BUFF_SIZE];

static void *checkout_handle(void *args)
{
    int id=*((int*)args);

    //Xử lý hàng 
    while(1){
        //Chờ đơn hàng 
        sem_wait(&checkout_sem[id]);
        //Dừng chờ khi đơn hàng được đọc (hàm sem_post() đại diện cho việc đọc được đơn hàng)
        //Nhập đơn hàng và xử lý
        printf("\nCheckout %d is processing an order: %s\n",id+1,order_buffer[id]);
        //sleep(2);

        //Thông bào xử lý thành công
        printf("Checkout %d has completed the order.\n",id+1);
    }
    return NULL;
}

int main(int agrc,const char *argv[])
{
    int fifo_fd;
    char buff_msg[BUFF_SIZE];
    pthread_t checkout_threads[NUM_CHECKOUTS];
    int checkout_data[NUM_CHECKOUTS];

    // Tạo FIFO nếu chưa tồn tại
    if (mkfifo(FIFO_NAME, 0666) == -1) {
        if (errno != EEXIST) {
            handle_error("mkfifo() failed");
        }
    }

    //Tạo semaphore cho mỗi quầy thanh toán 
    for(int i=0;i<NUM_CHECKOUTS;i++)
    {
        sem_init(&checkout_sem[i],0,0);
        checkout_data[i]=i;
        pthread_create(&checkout_threads[i],NULL,&checkout_handle,&checkout_data[i]);
    }

    //Mở file fifo để mở đơn hàng 
    int fd=open(FIFO_NAME,O_RDONLY);
    if(fd<0){
        handle_error("open()");
    }

    // Phân phối đơn hàng tới quầy thanh toán 
    int current_checkout = 0;
    while(read(fd,buff_msg,BUFF_SIZE)>0) {
        printf("\nCheckout Manager received: %s",buff_msg);

        //Gửi đơn hàng đến quầy thanh toán 
        strcpy(order_buffer[current_checkout],buff_msg);
        sem_post(&checkout_sem[current_checkout]);//Báo cho quầy bắt đầu xử lý đơn hàng 

        //Chuyển sang quầy tiếp theo 
        current_checkout = (current_checkout+1)%NUM_CHECKOUTS;
    }
    close(fifo_fd);

    //Đợi các quầy thanh toán hoàn thành
    for (int i = 0; i < NUM_CHECKOUTS; i++) {
        pthread_join(checkout_threads[i], NULL);
    }

    // Hủy semaphore
    for (int i = 0; i < NUM_CHECKOUTS; i++) {
        sem_destroy(&checkout_sem[i]);
    }

    return 0;
}