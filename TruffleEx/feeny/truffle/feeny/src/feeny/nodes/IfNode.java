package feeny.nodes;

import com.oracle.truffle.api.RootCallTarget;
import com.oracle.truffle.api.Truffle;
import com.oracle.truffle.api.frame.FrameDescriptor;
import com.oracle.truffle.api.frame.VirtualFrame;
import com.oracle.truffle.api.nodes.RootNode;

import feeny.Feeny;

public class IfNode extends RootNode {
    @Child RootNode pred_n;
    @Child RootNode then_n;
    @Child RootNode else_n;

    public IfNode(RootNode pred_, RootNode then_, RootNode else_, FrameDescriptor frameDescriptor) {
        super(Feeny.class, null, frameDescriptor);
        this.pred_n = pred_;
        this.then_n = then_;
        this.else_n = else_;
    }

    @Override
    public Object execute(VirtualFrame frame) {
        System.out.println("Evaluating " + this.getClass().getName());
        if (pred_n.execute(frame) == null) {
            return else_n.execute(frame);
        } else {
            return then_n.execute(frame);
        }
    }
}
