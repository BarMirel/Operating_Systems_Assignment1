#include "kernel/types.h"
#include "user/user.h"
static void
test_ping_pong(void)
{
  int parent_pid = getpid();
  int child_pid = fork();
  if(child_pid < 0){
    printf("co_test: fork failed\n");
    return;
  }
  if(child_pid == 0){
    for(int i = 0; i < 5; i++){
      int value = co_yield(parent_pid, 1);
      printf("co_test child received: %d\n", value);
      if(value < 0)
        break;
    }
    exit(0);
  }
  for(int i = 0; i < 5; i++){
    int value = co_yield(child_pid, 2);
    printf("co_test parent received: %d\n", value);
    if(value < 0)
      break;
  }
  wait(0);
}
static void
test_error_nonexistent_pid(void)
{
  int ret = co_yield(99999, 1);
  printf("co_test non-existent pid: %d (expected -1)\n", ret);
}
static void
test_error_self_yield(void)
{
  int ret = co_yield(getpid(), 1);
  printf("co_test self-yield: %d (expected -1)\n", ret);
}
static void
test_error_killed_target(void)
{
  int pid = fork();
  if(pid < 0){
    printf("co_test: fork failed in killed test\n");
    return;
  }
  if(pid == 0){
    sleep(100);
    exit(0);
  }
  kill(pid);
  sleep(1);
  int ret = co_yield(pid, 1);
  printf("co_test killed target: %d (expected -1)\n", ret);
  wait(0);
}
int
main(void)
{
  printf("co_test: ping-pong test start\n");
  test_ping_pong();
  printf("co_test: error tests start\n");
  test_error_nonexistent_pid();
  test_error_self_yield();
  test_error_killed_target();
  exit(0);
}