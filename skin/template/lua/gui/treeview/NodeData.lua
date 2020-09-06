local NodeData = FLua.Class("NodeData")
function NodeData:__constructor()
    self.level_ = 0
    self.folder_ = false
    self.child_visible_ = false
    self.has_child_ = false
    self.has_expand_ = false
    self.list_elment_ = nil --CListContainerElementUI 
end

return NodeData