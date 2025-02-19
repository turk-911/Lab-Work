def quick_sort(arr):
    if len(arr) <= 1:
        return arr
    pivot = arr[len(arr) // 2]
    left = [x for x in arr if x < pivot]
    middle = [x for x in arr if x == pivot]
    right = [x for x in arr if x > pivot]
    return quick_sort(left) + middle + quick_sort(right)

def merge_sort(arr):
    if len(arr) > 1:
        mid = len(arr) // 2
        left = arr[:mid]
        right = arr[mid:]

        merge_sort(left)
        merge_sort(right)

        i = j = k = 0
        while i < len(left) and j < len(right):
            if left[i] < right[j]:
                arr[k] = left[i]
                i += 1
            else:
                arr[k] = right[j]
                j += 1
            k += 1

        while i < len(left):
            arr[k] = left[i]
            i += 1
            k += 1

        while j < len(right):
            arr[k] = right[j]
            j += 1
            k += 1
    return arr

def insertion_sort(arr):
    for i in range(1, len(arr)):
        key = arr[i]
        j = i - 1
        while j >= 0 and key < arr[j]:
            arr[j + 1] = arr[j]
            j -= 1
        arr[j + 1] = key
    return arr

def read_file(filename):
    with open(filename, "r") as file:
        data = list(map(int, file.read().split()))
    return data

def append_to_file(filename, data):
    with open(filename, "a") as file:
        file.write("\n" + " ".join(map(str, data)))

def main():
    filename = "data.txt"
    data = read_file(filename)

    # Quick Sort
    sorted_data = quick_sort(data.copy())
    append_to_file(filename, sorted_data)

    # Merge Sort
    sorted_data = merge_sort(data.copy())
    append_to_file(filename, sorted_data)

    # Insertion Sort
    sorted_data = insertion_sort(data.copy())
    append_to_file(filename, sorted_data)

    print("Sorting completed and appended to the file.")

if __name__ == "__main__":
    main()