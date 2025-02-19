class Node:
    def __init__(self, data):
        self.data = data
        self.next = None

class LinkedList:
    def __init__(self):
        self.head = None

    def insert_at_beginning(self, data):
        new_node = Node(data)
        new_node.next = self.head
        self.head = new_node

    def insert_at_end(self, data):
        new_node = Node(data)
        if not self.head:
            self.head = new_node
            return
        temp = self.head
        while temp.next:
            temp = temp.next
        temp.next = new_node

    def insert_at_position(self, data, position):
        new_node = Node(data)
        if position == 1:
            new_node.next = self.head
            self.head = new_node
            return
        temp = self.head
        for _ in range(position - 2):
            if not temp:
                print("Invalid position!")
                return
            temp = temp.next
        new_node.next = temp.next
        temp.next = new_node

    def delete_by_value(self, value):
        if not self.head:
            print("List is empty!")
            return
        if self.head.data == value:
            self.head = self.head.next
            return
        temp = self.head
        while temp.next and temp.next.data != value:
            temp = temp.next
        if temp.next:
            temp.next = temp.next.next
        else:
            print(f"Value {value} not found!")

    def delete_by_position(self, position):
        if not self.head:
            print("List is empty!")
            return
        if position == 1:
            self.head = self.head.next
            return
        temp = self.head
        for _ in range(position - 2):
            if not temp.next:
                print("Invalid position!")
                return
            temp = temp.next
        if temp.next:
            temp.next = temp.next.next

    def search(self, value):
        temp = self.head
        position = 1
        while temp:
            if temp.data == value:
                print(f"Value {value} found at position {position}.")
                return
            temp = temp.next
            position += 1
        print(f"Value {value} not found!")

    def display(self):
        temp = self.head
        while temp:
            print(temp.data, end=" -> ")
            temp = temp.next
        print("NULL")

# Demonstration
if __name__ == "__main__":
    ll = LinkedList()
    ll.insert_at_beginning(10)
    ll.insert_at_end(20)
    ll.insert_at_position(15, 2)
    print("List after insertions:")
    ll.display()

    ll.delete_by_value(15)
    print("List after deleting value 15:")
    ll.display()

    ll.delete_by_position(2)
    print("List after deleting position 2:")
    ll.display()

    ll.search(10)
    ll.search(30)