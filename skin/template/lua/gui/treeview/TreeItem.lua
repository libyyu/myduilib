local TreeItem = FLua.Class("TreeItem")

local NodeData = require "gui.treeview.NodeData"

function TreeItem:__constructor()
    self.children_ = {}
    self.data_ = NodeData()
    self.parent_ = nil
    self.node_xml = ""
    self.tree = nil
end
function TreeItem:__destructor() 
end
function TreeItem:toString()
    return string.format("[%s] num_children:%d, folder:%s", self:GetPointer(), self:num_children(), tostring(self:folder()))
end
function TreeItem:data()
    return self.data_
end
function TreeItem:num_children()
    return #(self.children_)
end
function TreeItem:child(index)
    return self.children_[index]
end
function TreeItem:has_children()
    return self:num_children() > 0
end
function TreeItem:add_child(child)
    self:data().has_child_ = true
    child:set_parent(self)
	table.insert(self.children_, child) 
end
function TreeItem:remove_child(child)    
    for i=1, self:num_children() do
        if child == self:child(i) then
            table.remove(self.children_, i)
            break
        end
    end
end
function TreeItem:get_last_child()
    if self:has_children() then
		return self:child(self:num_children()):get_last_child()
	end
	return self
end
function TreeItem:parent()
    return self.parent_
end
function TreeItem:folder()
    return self.data_.folder_
end
function TreeItem:set_parent(parent)
    self.parent_ = parent
end

function TreeItem:_Create(tree)
    local dlg = DuiLib.CDialogBuilder()
    local pListElement = dlg:Create(self.node_xml, tree:GetManager())
    assert(pListElement and pListElement:IsClass("ListContainerElement"))
    if pListElement == nil then
        return false
    end

    self:data().list_elment_ = pListElement
    return true
end

function TreeItem:OnCreate()
    local pListElement = self:data().list_elment_
end

function TreeItem:AddToTree(tree, parent)
    if not self:_Create(tree) then
        return false
    end
    self:data().level_ = parent:data().level_ + 1

    local pListElement = self:data().list_elment_
    
    if not parent:data().child_visible_ then
        pListElement:SetVisible(false)
    end

    if parent ~= tree:GetRoot() and not parent:data().list_elment_:IsVisible() then
        pListElement:SetVisible(false)
    end

    local rcPadding = {left=0,top=0,right=0,bottom=0}
	for i = 1, self:data().level_ - 1 do
		rcPadding.left = rcPadding.left + tree:GetLevelTextStartPos()
    end
    self:SetPadding(rcPadding)

    local index = 0
    local prev = parent:get_last_child()

    if prev:data().list_elment_ then
        index = tree:GetItemIndex(prev) + 1
    end

    if not tree:GetTreeCtrl():AddAt(pListElement, index) then
        pListElement:Delete()
        pListElement = nil
        self:data().list_elment_ = nil
        return false
    end
    
    pListElement:SetUserData2("node", self)
    parent:add_child(self)
    self.tree = tree

    self:OnCreate()

    return true
end

function TreeItem:SetPadding(rcPadding)
    local pListElement = self:data().list_elment_
    pListElement:SetPadding(rcPadding)
end

function TreeItem:SetChildVisible(visible)
    self.tree:SetChildVisible(self, visible)
end

function TreeItem:SetVisible(visible)
    self.tree:SetNodeVisible(self, visible)
end

function TreeItem:CanExpand()
    return self.tree:CanExpand(self)
end

return TreeItem