/**
 * @brief 检查 fifo 管道是否被创建；
          若没有创建 fifo1 和 fifo2，则尝试创建这两个有名管道
 * 
 * @return int 创建成功或已存在有名管道则返回1，否则返回0
 */
int check_fifo();
