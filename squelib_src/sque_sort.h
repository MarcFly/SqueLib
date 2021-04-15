#ifndef SQUE_SORT_ALGORITHMS
#define SQUE_SORT_ALGORITHMS

// I wanted to have a quick sort for sque_vec free indices, so here is full implementation for learning

// Why order from small to big? 
// - Same things, indices I feel are tidier from small to big , probably because that is the norm. Just change the implementation, should be easy.

// Why do it? 
// - Because I didn't understand function complexity / complexity theory and also wanted to have sorts for sque_vec. At each sort it is explained complexity and why
// which is a thing I have struggled for a long time coming from a games degree instead of a computer science one...

// Are they any good?
// - Fuck no, I don't think any of these implementations are good but they get the point of the algorithm I hope.
// - If you want to check performance, there is a part in the performance example function with 1.000 to 100.000 value sorting, 100.000 takes a LOT OF TIME in some algorithms)

// Basics of understanding complexity
// O / f(o) -> Big O / Function of O -> is just jargon to say that the complexity of the following function is based on the following mathematical function
// N -> Number of elements to iterate over
// Example: O(N*2) -> The function iterates 2 times over N elements
// Usually most algorithms iterate over each element N times, the iteration over a value might mean literal N times, or N - n -> n is step we are in
// Maybe thats not how it is please explain to me if it is not correct interpretation...

// Selection Sort:
// - Find Smallest Element in the elements after N
// - Put the smallest found into N and N where smallest was found
// O(N^2) -> We iterate over all elements up to N times each time

template<class T>
void SQUE_Swap(T* v1, T* v2)
{
    T intermediary;
    memcpy(&intermediary, v1, sizeof(T));
    memcpy(v1, v2, sizeof(T));
    memcpy(v2, &intermediary, sizeof(T));
}

template<class T> 
void SQUE_SORT_Selection(T* arr, uint32_t size)
{
    uint32_t smallest_index;
    T intermediary;

    for(uint32_t i = 0; i < size; ++i)
    {
        smallest_index = i;
        for (uint32_t j = i + 1; j < size; ++j)
        {
            if (arr[j] < arr[smallest_index])
            {
                smallest_index = j;
            }
        }

        SQUE_Swap<T>(&arr[i], &arr[smallest_index]);
    }
}

// Bubble Sort:
// - Compare Each element with its neighbour
// - Swap if Neighbour if smaller
// - Repeat For all elements excepth the last
// - Repeat until in a pass there were no swaps
// O(N^2) -> Evey time you perform a pass, we need one less iteration on the pass, still you have to iterate over all N elements N times at max

template<class T>
void SQUE_SORT_Bubble(T* arr, uint32_t size)
{
    for (uint32_t steps = 0; steps < size - 1; ++steps)
    {
        for (uint32_t i = 0; i < size - steps - 1; ++i)
        {
            if (arr[i + 1] < arr[i])
            {
                SQUE_Swap<T>(&arr[i], &arr[i + 1]);
            }
        }
    }
}

// Recursive Implementation, because it is possible, just don't do it just makes sense if you want a bubble sort step somehow

template<class T>
void SQUE_SORT_BubbleRecursive(T* arr, uint32_t size)
{
    for (uint32_t i = 0; i < size - 1; ++i)
    {
        if (arr[i + 1] < arr[i])
        {
            SQUE_Swap<T>(&arr[i], arr[i + 1]);
        }
    }

    SQUE_SORT_BubbleRecursive(arr, size - 1);
}

// Insertion Sort:
// - Start Element 1
// - Check Predecessor - If smaller, move all elements one back and put Element there
// - if not check next predecessor until 0 for swaps
// - Repeat until you are starting at Size
// O(N^2) At Worst, O(N) At Best
// At worst each element is completely inverted and has to traverse back all numbers to check
// [3 , 2, 1]
// At best each time you swap it is the closest position correct, basically ordered shifted once to the right 
// [3, 1, 2] // [5,1,2,3,4]

template<class T>
void SQUE_SORT_Insertion(T* arr, uint32_t size)
{
    for (uint32_t i = 1; i < size; ++i)
    {
        
        for (int64_t j = i - 1; j >= 0; --j)
        {
            if (arr[j+1] > arr[j]) break;
            SQUE_Swap<T>(&arr[j], &arr[j + 1]);
        }
    }
}

template<class T>
void SQUE_SORT_InsertionRecursive(T* arr, int64_t size)
{
    if (size < 1) return;
    SQUE_SORT_InsertionRecursive(arr, size - 1);

    T intermediary;
    for (int64_t j = size - 1; j >= 0; --j)
    {
        if (arr[j + 1] > arr[j]) break;
        SQUE_Swap<T>(&arr[j], &arr[j + 1]);
    }
}

// Merge Sort: Recursive splits by 2 each time
// - Divide Array in 2 parts
// - Repeat until divided into arrays of 1 value
// - Merge compares the values in 2 arrays one by one
// - Insert the smaller into the final arrau and then compare the next against value that was not inserted
// - After you reach end of any of the 2 arrays, copy what is left of the other array
// O(n*log(n)) -> You iterate over N values, then the amount of half arrays is based on log(n), how many times the array can be halved
// https://softwareengineering.stackexchange.com/questions/297160/why-is-mergesort-olog-n#:~:text=The%20complexity%20of%20merge%20sort,and%20NOT%20O(logn).&text=The%20divide%20step%20computes%20the,for%20even%20n)%20elements%20each.

template<class T>
void merge(T* arr, uint32_t start, uint32_t middle, uint32_t end)
{
    uint32_t size1(middle - start);
    uint32_t size2(end - middle);

    T* arr1 = (T*)malloc(sizeof(T)*size1);
    T* arr2 = (T*)malloc(sizeof(T)*size2);

    memcpy(arr1, arr+start, sizeof(T) * size1);
    memcpy(arr2, arr+middle, sizeof(T) * size2);

    uint32_t left_it = 0, right_it = 0;
    uint32_t arr_it = start;
    while(left_it < size1 && right_it < size2)
    {
        if (arr1[left_it] < arr2[right_it])
            memcpy(&arr[arr_it], &arr1[left_it++], sizeof(T));
        else
            memcpy(&arr[arr_it], &arr2[right_it++], sizeof(T));

        ++arr_it;
    }

    while (left_it < size1)
    {
        memcpy(&arr[arr_it], &arr1[left_it++], sizeof(T));
        ++arr_it;
    }

    while (right_it < size2)
    {
        memcpy(&arr[arr_it], &arr2[right_it++], sizeof(T));
        ++arr_it;
    }

    free(arr1);
    free(arr2);
}

template<class T>
void SQUE_SORT_Merge(T* arr, uint32_t end, uint32_t start = 0)
{
    if ((end-(int32_t)start) < 2) 
        return;
    uint32_t size = end - start;
    uint32_t middle = end - size / 2;
    SQUE_SORT_Merge(arr, middle, start);
    SQUE_SORT_Merge(arr, end, middle);
    merge<T>(arr, start, middle, end);
}

// Quick Sort:
// - Pick a position in the array
// - Put this pivot to the end
// - Compare the left most value with pivot
// - If pivot is smaller, then swap pivot with neigbour and neighbour again with the left most value
// - Ex: [3, 2, 1] -> Pivot is 1 -> [3, 2, 1] -> Test 3 against 1, which means swap 1 with 2
//   [3, 1, 2] -> Then Swap 2 with 3 -> [2,1,3] - > Now pivot is 1 to the left and left most value still has to be checked
// - When doing a swap it is important to keep left value to where it was as we have done an inplace swap
// - If the value is smaller than pivot then get next value to test with pivot
// - When finished, because of how I've done the swap is not done correctly on even arrays
//      You check that those 2 values are in place, right is pivot, left is not sorted -> swap if they are not in order
// - Return the right value, which is where pivot is
// - Recursive that with the current start as start and pivot as the new end, and another step with pivot as the new start and the current end as new end
// O(n*log(n)) -> Similar to merge sort, it is bound for at max the number of partitions possible

template<class T>
uint32_t partition(T* arr, uint32_t start, uint32_t end)
{

    uint32_t num_pivot = start + ((end - start) / 2 - 1);
    SQUE_Swap<T>(&arr[end - 1], &arr[num_pivot]);
    num_pivot = end - 1;

    // Iterate over all values
    uint32_t right = num_pivot;
    uint32_t left = start;
    while(left < (right-1))
    {
        if (arr[left] > arr[right])
        {
            SQUE_Swap<T>(&arr[right - 1], &arr[right]);
            SQUE_Swap<T>(&arr[right--], &arr[left--]);
        }
        ++left;
    }

    if (arr[left] > arr[right])
    {
        SQUE_Swap<T>(&arr[right--], &arr[left]);
    }

    return right;
}

template<class T>
void SQUE_SORT_Quick(T* arr, uint32_t end, uint32_t start = 0)
{
    if ((int32_t)start < ((int32_t)end-1))
    {
        uint32_t pivot = partition(arr, start, end);
        SQUE_SORT_Quick<T>(arr, pivot, start);
        SQUE_SORT_Quick<T>(arr, end, pivot+1);
    }
}

#endif