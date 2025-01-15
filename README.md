# **Tic-Tac-Toe Engine**

An advanced implementation of the classic Tic-Tac-Toe game in C, designed with modern computational techniques and efficient data structures. This project provides an engaging, interactive gaming experience with features like adaptive AI, multiplayer access, real-time undo functionality, and dynamic player statistics.

---

## **Features**
- **Customizable AI:**
  - Easy Mode: Random moves.
  - Medium Mode: Decision-tree-based strategy.
  - Impossible Mode: Unbeatable AI using the Minimax Algorithm.
- **Multiplayer Access:** Two players can compete, with their scores dynamically tracked.
- **Undo Functionality:** Reverse the last move for both player and AI using a stack.
- **Player Statistics:** Tracks wins, losses, and draws using a hash table.
- **Dynamic Game Sessions:** Stores past game data using linked lists.

---

## **Data Structures Used**
- **Array:** Represents the 3x3 game board for constant-time access and updates.
- **Stack:** Implements undo functionality by tracking the sequence of moves.
- **Linked List:** Dynamically stores game session details for efficient memory usage.
- **Hash Table:** Maintains player statistics with fast lookups and updates.
- **Decision Tree:** Used in AI modes for strategic move evaluation.

---

## **Techniques Implemented**
1. **Minimax Algorithm:** Ensures optimal AI decisions by evaluating all possible moves.
2. **Decision Tree:** Simulates potential game states for AI strategy.
3. **Dynamic Memory Management:** Linked lists dynamically track game sessions.
4. **Efficient Undo:** LIFO-based stack tracks moves for seamless reversal.

---

## **How to Run**
1. Clone the repository:
   ```bash
   git clone https://github.com/your-username/tic-tac-toe-engine.git
