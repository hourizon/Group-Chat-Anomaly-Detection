# Group Chat Anomaly Detection Engine (群聊异常行为检测引擎)

This project is an intelligent detection engine built from scratch in C++ to identify anomalous behavior in group chats. It is designed to showcase the ability to implement security-oriented algorithms in a low-level language, making it a strong portfolio piece for network and system security roles.

The engine connects to a QQ Bot framework (like go-cqhttp) and analyzes chat messages and user activities in real-time.

---

## 核心功能 (Core Features)

- **Content-based Security (内容安全)**: Implements a Naive Bayes classifier from scratch to analyze chat messages and calculate the probability of them being spam or malicious.
- **Behavior-based Security (行为安全)**: Implements a statistical profiler to monitor user actions (e.g., frequency of joining groups) and detect behavior that deviates significantly from the norm.
- **C++ from Scratch (C++ 从零实现)**: All core detection algorithms are implemented in pure C++, without reliance on external machine learning libraries, demonstrating strong C++ and algorithmic skills.
- **Bot Integration (机器人集成)**: Designed to integrate with a `go-cqhttp` instance to receive events and execute moderation actions (e.g., kicking or muting users).

---

## 技术架构 (Architecture)

The system consists of two main parts:

1.  **The C++ Detection Engine**: This is the core of the project. It includes:
    - A **Trainer** executable to build the Naive Bayes model from sample data (`ham.txt`, `spam.txt`).
    - The main **Bot** executable that loads the trained model, connects to the QQ Bot framework, and performs real-time analysis.
2.  **go-cqhttp Framework**: Acts as a bridge to the QQ network, forwarding events to the C++ engine and executing commands received from it.

```
+-------------------------+      +-------------------------+
|      Training Data      |      |        QQ Server        |
| (spam.txt, ham.txt)     |      |      (Events API)       |
+-------------------------+      +-------------------------+
          | (C++ Trainer)                ^      |
          v                              |      | (Commands)
+-------------------------+      |      v
|       Model File        |      |      +-------------------------+
|   (bayes_model.dat)     |      |      |   QQ Bot (go-cqhttp)    |
+-------------------------+      |      +-------------------------+
          ^                              ^
          | (C++ Classifier)             | (Events)
          |                              |
+-------------------------------------------------+
|         C++ Anomaly Detection Engine            |
|                                                 |
|  +------------------+  +---------------------+  |
|  | NaiveBayes Module|  | BehaviorProfile Module|  |
|  +------------------+  +---------------------+  |
+-------------------------------------------------+
```

---

## 如何开始 (Getting Started)

### Prerequisites

- A C++ compiler (like g++)
- `make`
- An instance of [go-cqhttp](https://github.com/Mrs4s/go-cqhttp) running and configured.

### Build

You can compile the project using the provided Makefile:

```bash
make
```

This will generate two main executables:
- `bayes_classifier`: The model trainer.
- `bot_main`: The main bot client application.

### Training

Before running the bot, you need to train the Naive Bayes classifier:

1.  Prepare your training data in `data/ham.txt` (normal messages) and `data/spam.txt` (spam messages).
2.  Run the trainer:
    ```bash
    ./bayes_classifier
    ```
3.  This will generate a `bayes_model.dat` file in the `data/` directory.

### Running the Bot

Once the model is trained, you can run the main application:

```bash
./bot_main
```

The bot will attempt to connect to your `go-cqhttp` instance (you may need to configure the connection details in the source code, e.g., in `src/bot_main.cpp`).

---

## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.
