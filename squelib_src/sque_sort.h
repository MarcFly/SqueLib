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

        memcpy(&intermediary, &arr[i], sizeof(T));
        memcpy(&arr[i], &arr[smallest_index], sizeof(T));
        memcpy(&arr[smallest_index], &intermediary, sizeof(T));
    }
}

// Bubble Sort:
// - Compare Each element with its neighbour
// - Swap if Neighbour if smaller
// - Repeat For all elements excepth the last
// - Repeat until in a pass there were no swaps
// O(N^2) -> Evey time you perform a pass, we need one less iteration on the pass, still you have to iterate over all N elements N times at max

template<class T>
void SQUE_SORT_Bubble(uint32_t* arr, uint32_t size)
{
    uint32_t intermediary;
    for (uint32_t steps = 0; steps < size - 1; ++steps)
    {
        for (uint32_t i = 0; i < size - steps - 1; ++i)
        {
            if (arr[i + 1] < arr[i])
            {
                memcpy(&intermediary, &arr[i], sizeof(uint32_t));
                memcpy(&arr[i], &arr[i + 1], sizeof(uint32_t));
                memcpy(&arr[i + 1], &intermediary, sizeof(uint32_t));
            }
        }
    }
}

// Recursive Implementation, because it is possible, just don't do it just makes sense if you want a bubble sort step somehow

template<class T>
void SQUE_SORT_BubbleRecursive(T* arr, uint32_t size)
{

    T intermediary;
    for (uint32_t i = 0; i < size - 1; ++i)
    {
        if (arr[i + 1] < arr[i])
        {
            memcpy(&intermediary, &arr[i], sizeof(T));
            memcpy(&arr[i], &arr[i + 1], sizeof(T));
            memcpy(&arr[i + 1], &intermediary, sizeof(T));
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
    T intermediary;
    for (uint32_t i = 1; i < size; ++i)
    {
        
        for (int64_t j = i - 1; j >= 0; --j)
        {
            if (arr[j+1] > arr[j]) break;
            memcpy(&intermediary, &arr[j], sizeof(T));
            memcpy(&arr[j], &arr[j + 1], sizeof(T));
            memcpy(&arr[j + 1], &intermediary, sizeof(T));
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
        memcpy(&intermediary, &arr[j], sizeof(T));
        memcpy(&arr[j], &arr[j + 1], sizeof(T));
        memcpy(&arr[j + 1], &intermediary, sizeof(T));
    }
}

// Merge Sort: Recursive splits by 2 each time
// - Divide Array in 2 parts
// - Repeat until divided into arrays of 1 value
// - Compare the values in each array and sort them, then put them together
// ????

template<class T>
void SQUE_SORT_Merge(T* arr, uint32_t size)
{
    if (size == 1) return;
    T arr1[size / 2];
    T arr2[size - (size / 2)];
    memcpy(arr1, arr, (size / 2) * sizeof(T));
    memcpy(arr2, arr + (size / 2), (size - (size / 2)) * sizeof(T));

    SQUE_SORT_Merge(arr1, size / 2);
    SQUE_SORT_Merge(arr2, size - (size / 2));


}