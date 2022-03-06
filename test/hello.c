

int func()
{
    int t = 0;
    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 10; j++)
            t += i;
    }
    return t;
}

int func2(int h, int f)
{
  int ans = func();
  return ans * h * f;
}

int getResult(int h, int f)
{
  int ans = 0;
  if (func() < 10) {
    ans = func2(h, f);
  } else {
    ans = func2(0,0);
  }

}
