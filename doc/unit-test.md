# WebServ Unit Testing Guide

This document explains how to add and run unit tests for the WebServ project.

---

## 1. Test Structure

- All test code is located in the `test/` directory.
- Each test file (e.g., `http.cpp`) contains one or more test functions.
- The test framework uses a `TestSuite` class to register and run tests.

---

## 2. Writing a Test

1. **Create a test function**
   - The function must return `bool` (`true` for pass, `false` for fail).
   - Use the `assertEqual` function to compare actual and expected results.

   **Example:**
   ```cpp
   static bool test_example()
   {
       std::string actual = "foo";
       std::string expected = "foo";
       return assertEqual("example test", actual, expected);
   }
   ```

2. **Register the test**
   - In your test suite registration function (e.g., `test_http(TestSuite &t)`), add your test:
   ```cpp
   void test_http(TestSuite &t)
   {
       t.addTest(test_example);
       // Add other tests here
   }
   ```

---

## 3. Running Tests

- The main test runner is in `test/main.cpp`:
  ```cpp
  int main()
  {
      TestSuite t;
      test_http(t); // Register HTTP tests
      t.run();      // Run all registered tests
      return 0;
  }
  ```
- To run all tests, use the Makefile:
  ```sh
  make test
  ```

---

## 4. Test Output

- Test results are printed to the terminal.
- Each test prints `[PASS]` or `[FAIL]` with a message.
- A summary is shown at the end.

---

## 5. Tips

- Use descriptive messages in `assertEqual` for easier debugging.
- Group related tests in the same file and registration function.
- Only use `static bool` functions for tests to avoid symbol conflicts.

---

## 6. Example: Adding a New Test

Suppose you want to test a function `int add(int, int)`:

1. **Write the test:**
   ```cpp
   static bool test_add()
   {
       int actual = add(2, 3);
       int expected = 5;
       return assertEqual("add(2, 3) == 5", actual, expected);
   }
   ```

2. **Register the test:**
   ```cpp
   void test_math(TestSuite &t)
   {
       t.addTest(test_add);
   }
   ```

3. **Update `main.cpp`:**
   ```cpp
   int main()
   {
       TestSuite t;
       test_math(t);
       t.run();
   }
   ```

---

Happy testing!
