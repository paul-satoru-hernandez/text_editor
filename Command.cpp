#include "Command.h"
#include "ECTextViewImp.h"
#include "TextControl.h"
#include <iostream>


// *******************************************************************
// Implementing Commands
InsertTextAtCmd :: InsertTextAtCmd(TextDocument &doc, int x, int y, char ch) : doc(doc), x(x), y(y), ch(ch)
{
}

void InsertTextAtCmd :: Execute()
{
    if (doc.GetRows().empty()) {
        doc.Reset();
    }
    doc.InsertCharAt(x, y, ch);
}

void InsertTextAtCmd :: UnExecute()
{
    doc.RemoveCharAt(x, y);
}

RemoveTextAtCmd :: RemoveTextAtCmd(TextDocument &doc, int x, int y) : doc(doc), x(x), y(y)
{
}

void RemoveTextAtCmd :: Execute()
{
    std::vector<std::string> Rows = doc.GetRows();
    if (x == 0 && y > 0) {
        prevRow = Rows[y-1];
        currRow = Rows[y];
        std::string newRow = prevRow + currRow;
        doc.RemoveRow(y-1);
        doc.InsertRow(newRow, y-1);
        doc.RemoveRow(y);
    } else if (x > 0) {
        removed = doc.GetCharAt(x-1, y);
        doc.RemoveCharAt(x-1, y);
    }
}

void RemoveTextAtCmd :: UnExecute()
{
    if (x == 0 && y > 0) {
        doc.InsertRow(prevRow, y);
        doc.RemoveRow(y-1);
        doc.InsertRow(currRow, y);
    } else if (x > 0) {
        doc.InsertCharAt(x-1, y, removed);
    }
}

InsertRowCmd :: InsertRowCmd(TextDocument &doc, int x, int y) : doc(doc), x(x), y(y)
{
}

void InsertRowCmd :: Execute()
{
    std::vector<std::string> Rows = doc.GetRows();
    right = Rows[y].substr(0, x);
    left = Rows[y].substr(x, Rows[y].size()-x);
    doc.RemoveRow(y);
    doc.InsertRow(left, y);
    doc.InsertRow(right, y);
}

void InsertRowCmd :: UnExecute()
{
    doc.RemoveRow(y);
    doc.RemoveRow(y);
    std::string row = right + left;
    doc.InsertRow(row, y);
}

PasteCmd :: PasteCmd(TextDocument &doc, int x, int y, std::string copiedText) : doc(doc), x(x), y(y), copiedText(copiedText)
{
}

void PasteCmd :: Execute()
{
    std::vector<std::string> Rows = doc.GetRows();
    right = Rows[y].substr(0, x);
    left = Rows[y].substr(x, Rows[y].size()-x);
    doc.InsertRow(right + copiedText + left, y);
    doc.RemoveRow(y+1);
}

void PasteCmd :: UnExecute()
{
    doc.RemoveRow(y);
    doc.InsertRow(right + left, y);
}
// *******************************************************************


// *******************************************************************
// Command History Implementation
CommandHistory :: CommandHistory ()
{
}

CommandHistory :: ~CommandHistory()
{
}

bool CommandHistory :: Undo()
{
    if (listCmds.empty()) {
        return false;
    }
    Command *pCmd = listCmds.back();
    if (pCmd->GetType() == "Edge") {
        listUndoneCmds.push_back(pCmd);
        listCmds.pop_back();
        pCmd = listCmds.back();
        while (pCmd->GetType() != "Edge" && !listCmds.empty()) {
            pCmd->UnExecute();
            listUndoneCmds.push_back(pCmd);
            listCmds.pop_back();
            pCmd = listCmds.back();
        }
    }
    pCmd->UnExecute();
    listUndoneCmds.push_back(pCmd);
    listCmds.pop_back();
    return true;
}

bool CommandHistory :: Redo()
{
    if (listUndoneCmds.empty()) {
        return false;
    }
    Command *pCmd = listUndoneCmds.back();
    if (pCmd->GetType() == "Edge") {
        listCmds.push_back(pCmd);
        listUndoneCmds.pop_back();
        pCmd = listUndoneCmds.back();
        while (pCmd->GetType() != "Edge" && !listCmds.empty()) {
            pCmd->Execute();
            listCmds.push_back(pCmd);
            listUndoneCmds.pop_back();
            pCmd= listUndoneCmds.back();
        }
    }
    pCmd->Execute();
    listCmds.push_back(pCmd);
    listUndoneCmds.pop_back();
    return true;
}

void CommandHistory :: ExecuteCmd(Command *pCmd)
{
    pCmd->Execute();
    listCmds.push_back(pCmd);
}
// *******************************************************************
