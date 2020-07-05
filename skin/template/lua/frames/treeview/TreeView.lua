local TreeItem = require "frames.treeview.TreeItem"
local TreeView = FLua.Class("TreeView")

function TreeView.new(pCtrl)
    assert(pCtrl ~= nil and pCtrl:IsClass("List"), "pCtrl is nil")
    local obj = TreeView()
    obj._pTreeViewCtrl = pCtrl
    obj:OnInit()
	return obj
end
function TreeView:__constructor()
    self._pTreeViewCtrl = nil
    self.root_node_ = nil
	self.delay_deltaY_ = nil
	self.delay_number_ = nil 
	self.delay_left_ = nil
	self.text_padding_ = {left=0, top=0, right=0, bottom=0}
	self.level_text_start_pos_ = 5
	self.level_expand_image_ = "<i res/list/list_icon_b.png>"
	self.level_collapse_image_ = "<i res/list/list_icon_a.png>"
end
function TreeView:__destructor()
   warn("TreeView:__destructor")
end

function TreeView:OnInit()
    self._pTreeViewCtrl:SetItemShowHtml(true)

    self.root_node_ = TreeItem.new()
	self.root_node_:data().level_ = 0
	self.root_node_:data().child_visible_ = true
	self.root_node_:data().has_child_ = true
	self.root_node_:data().list_elment_ = nil
end

function TreeView:GetRoot()
    return self.root_node_
end

function TreeView:AddFolder(name, parent)
    if not parent then
        parent = self.root_node_
    end
    
    local node_xml = "xml/item/node_0.xml"
    local dlg = DuiLib.CDialogBuilder()
    local pListElement = dlg:Create(node_xml, self._pTreeViewCtrl:GetManager())
    assert(pListElement and pListElement:IsClass("ListContainerElement"))
    if pListElement == nil then
        return nil
    end

    local node = TreeItem.new()
	node:data().level_ = parent:data().level_ + 1
	node:data().child_visible_ = false
	node:data().has_child_ = true
    node:data().list_elment_ = pListElement
    node:data().folder_ = true

    if not parent:data().child_visible_ then
        pListElement:SetVisible(false)
    end

    if parent ~= self.root_node_ and not parent:data().list_elment_:IsVisible() then
        pListElement:SetVisible(false)
    end

    local rcPadding = {left=self.text_padding_.left,top=self.text_padding_.top,right=self.text_padding_.right,bottom=self.text_padding_.bottom}
	for i = 1, node:data().level_ - 1 do
		rcPadding.left = rcPadding.left + self.level_text_start_pos_
    end
    pListElement:SetPadding(rcPadding)
    
    local index = 0
    local prev = parent:get_last_child()

    if prev:data().list_elment_ then
        index = self._pTreeViewCtrl:GetItemIndex(prev:data().list_elment_) + 1
    end

    if not self._pTreeViewCtrl:AddAt(pListElement, index) then
        pListElement:Delete()
        pListElement = nil
        node = nil
        return nil
    end
    
    pListElement:SetUserData2("node", node)
    parent:add_child(node)

    local nameObj = pListElement:FindSubControl("tree_name")
    local checkObj = pListElement:FindSubControl("tree_expandbtn")
    nameObj:SetTagCtrl(pListElement)
    checkObj:SetTagCtrl(pListElement)
    nameObj:OnNotifyAdd(function(msg)
        if msg.sType == "dbclick" then
            if self:CanExpand(node) then
                self:SetChildVisible(node, not node:data().child_visible_)
            end
        end
    end)
    checkObj:OnNotifyAdd(function(msg)
        if msg.sType == "selectchanged" then
            if self:CanExpand(node) then
                self:SetChildVisible(node, not node:data().child_visible_)
            end
        end
    end)

    nameObj:SetText(name)

    return node
end

function TreeView:AddNode(node_xml, parent)
    local tempNode = parent
    if not parent then
        parent = self.root_node_
    end

    local dlg = DuiLib.CDialogBuilder()
    local pListElement = dlg:Create(node_xml, self._pTreeViewCtrl:GetManager())
    assert(pListElement and pListElement:IsClass("ListContainerElement"))
    if pListElement == nil then
        return nil
    end

    local node = TreeItem.new()
	node:data().level_ = parent:data().level_ + 1
	node:data().child_visible_ = true
	node:data().has_child_ = false
    node:data().list_elment_ = pListElement

    if not parent:data().child_visible_ then
        pListElement:SetVisible(false)
    end

	if parent ~= self.root_node_ and not parent:data().list_elment_:IsVisible() then
        pListElement:SetVisible(false)
    end

	local rcPadding = {left=self.text_padding_.left,top=self.text_padding_.top,right=self.text_padding_.right,bottom=self.text_padding_.bottom}
	for i = 1, node:data().level_ do
		rcPadding.left = rcPadding.left + self.level_text_start_pos_
    end
    pListElement:SetPadding(rcPadding)
       
    local index = 0
	local prev = parent:get_last_child()
	if prev:data().list_elment_ then
        index = self._pTreeViewCtrl:GetItemIndex(prev:data().list_elment_) + 1
    end

    if not self._pTreeViewCtrl:AddAt(pListElement, index) then
        pListElement:Delete()
        pListElement = nil
		node = nil
    end

    pListElement:SetUserData2("node", node)
    parent:add_child(node)
        
    return node
end

function TreeView:RemoveNode(node)
    if not node or node == self.root_node_ then
        return false
    end
    for i=node:num_children(), 1, -1 do
		local child = node:child(i)
		self:RemoveNode(child)
    end

    self._pTreeViewCtrl:Remove(node:data().list_elment_)
	node:parent():remove_child(node)
    node = nil
    
	return true
end

function TreeView:SetChildVisible(node, visible)
    if not node or node == self.root_node_ then
        return
    end

    if node:data().child_visible_ == visible then
        return
    end

    node:data().child_visible_ = visible
    
    if node:data().has_child_ then
    end

    if not node:data().list_elment_:IsVisible() then
        return
    end

    if not node:has_children() then
        return
    end

    local cbegin = node:child(1)
    local cend = node:get_last_child()

    for i = cbegin:data().list_elment_:GetIndex(), cend:data().list_elment_:GetIndex() do
        local control = self._pTreeViewCtrl:GetItemAt(i)
		if control:IsClass(_T("ListContainerElement")) then
            if visible then
                local cnode = control:GetUserData2("node")
				local local_parent = cnode:parent()
				if local_parent:data().child_visible_ and local_parent:data().list_elment_:IsVisible() then
					control:SetVisible(true)
                end 
			else
				control:SetVisible(false)
            end
		end
	end
end

function TreeView:CanExpand(node)
    if not node or node == self.root_node_ then
        return false
    end

	return node:data().has_child_
end

return TreeView