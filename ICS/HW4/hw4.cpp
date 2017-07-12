    void bubble_b(long *data,long count)
    {
        long i,last,t;
        for(last = count - 1; last > 0; last--)
        {
            for(i = 0; i < last; i++)
            {
                if( *(data + i) > *(data + i + 1))
              {
                t = *(data + i);
                *(data + i) = *(data + i + 1);
                *(data + i + 1) = t;
              }
            }
        }
    }

int main()
{
    return 0;

}
