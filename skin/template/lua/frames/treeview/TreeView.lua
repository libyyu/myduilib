local TreeItem = require "frames.treeview.TreeItem"
local TreeView = FLua.Class("TreeView")

local FolderItem = FLua.Class(TreeItem, "FolderItem")
do
    function FolderItem.new(xml, name)
        local obj = FolderItem()
        obj.name_ = name
        obj.node_xml = xml
        return obj
    end
    function FolderItem:__constructor()
        self.name_ = ""
        self.checked_ = false
    end
    function FolderItem:OnCreate()
        TreeItem.OnCreate(self)
        local pListElement = self:data().list_elment_
        local nameObj = pListElement:FindSubControl("tree_name")
        local checkObj = pListElement:FindSubControl("tree_expandbtn")
        local chooseObj = pListElement:FindSubControl("tree_choose")
        nameObj:SetTagCtrl(pListElement)
        checkObj:SetTagCtrl(pListElement)
        chooseObj:SetTagCtrl(pListElement)
        nameObj:OnNotifyAdd(function(msg)
            if msg.sType == "dbclick" then
                if self:CanExpand() then
                    self:SetChildVisible(not self:data().child_visible_)
                end
            end
        end)
        checkObj:OnNotifyAdd(function(msg)
            if msg.sType == "selectchanged" then
                if self:CanExpand() then
                    self:SetChildVisible(not self:data().child_visible_)
                end
            end
        end)
        chooseObj:OnNotifyAdd(function(msg)
            if msg.sType == "selectchanged" then
                self:Choose(not self.checked_)
            end
        end)

        nameObj:SetText(self.name_)
    end

    function FolderItem:Choose(b)
        self.checked_ = b
        for i=1, self:num_children() do
            local child = self:child(i)
            if child:is(FolderItem) then
                child:Choose(b)
                local chooseObj = child:data().list_elment_:FindSubControl("tree_choose")
                chooseObj:SetCheck(b)
            end
        end
        if not b then
            local parent = self:parent()
            while parent do
                if parent:is(FolderItem) then
                    local chooseObj = parent:data().list_elment_:FindSubControl("tree_choose")
                    chooseObj:SetCheck(b)

                    parent.checked_ = false 
                end

                parent = parent:parent()
            end
        end 
    end
end
local TemplateItem = FLua.Class(TreeItem, "TemplateItem")
do
    function TemplateItem.new(xml, name)
        local obj = TemplateItem()
        obj.name_ = name
        obj.node_xml = xml
        return obj
    end
    function TemplateItem:__constructor()
        self.name_ = ""
    end
    function TemplateItem:OnCreate()
        TreeItem.OnCreate(self)
    end
end


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
	self.level_text_start_pos_ = 2
end
function TreeView:__destructor()
   warn("TreeView:__destructor")
end

function TreeView:OnInit()
    self._pTreeViewCtrl:SetItemShowHtml(true)

    self.root_node_ = TreeItem()
	self.root_node_:data().level_ = 0
	self.root_node_:data().child_visible_ = true
	self.root_node_:data().list_elment_ = nil
end

function TreeView:GetManager()
    return self._pTreeViewCtrl:GetManager()
end

function TreeView:GetRoot()
    return self.root_node_
end

function TreeView:GetLevelTextStartPos()
    return self.level_text_start_pos_
end

function TreeView:GetItemIndex(node)
    if node == self.root_node_ then return -1 end
    assert(self._pTreeViewCtrl)
    assert(node:data().list_elment_)
    return self._pTreeViewCtrl:GetItemIndex(node:data().list_elment_)
end 
function TreeView:GetTreeCtrl()
    return self._pTreeViewCtrl
end

function TreeView:Add(node, parent)
    if not parent then
        parent = self.root_node_
    end

    if not node:AddToTree(self, parent) then
        return nil
    end

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