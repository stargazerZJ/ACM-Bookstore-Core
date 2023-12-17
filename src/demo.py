import bookstore

store = bookstore.BookStore("db/")
store.initialize(force_reset=True)

# login
print("Login with username: root, password: sjtu")
code = store.login("root", "sjtu")
print(bookstore.exceptionTypeToString(code))

# buy a book
print("Buy a book")
ret = store.purchase("978-7-302-32998-2", 1)
code, record = ret.first, ret.second
print(bookstore.exceptionTypeToString(code))
print(record)

# show finance
print("Show finance")
ret = store.showFinance()
code, record = ret.first, ret.second
print(code, record.income(), record.expenditure())


# add a book
print("Add a book")
book = bookstore.Book()
book.ISBN = "978-7-302-32998-2"
book.title = "C++_Primer"
book.author = "Stanley_B._Lippman"
book.price = 8900 # in cents
print("Select book with ISBN:", book.ISBN)
code = store.select(book.ISBN)
print(bookstore.exceptionTypeToString(code))
print("Modify the book with new information")
book.ISBN = ""
code = store.modify(book)
print(bookstore.exceptionTypeToString(code))
print("Show all books")
empty_book = bookstore.Book() # search with empty book means show all books
ret = store.search(empty_book)
code, books = ret.first, ret.second
print(bookstore.exceptionTypeToString(code))
for book in books:
	print(book.ISBN, book.title, book.author, book.price, book.quantity)