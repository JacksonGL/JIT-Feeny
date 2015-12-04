package feeny.nodes;

import com.oracle.truffle.api.RootCallTarget;
import com.oracle.truffle.api.Truffle;
import com.oracle.truffle.api.frame.FrameDescriptor;
import com.oracle.truffle.api.frame.FrameSlot;
import com.oracle.truffle.api.frame.FrameSlotKind;
import com.oracle.truffle.api.frame.FrameSlotTypeException;
import com.oracle.truffle.api.frame.VirtualFrame;
import com.oracle.truffle.api.nodes.RootNode;

import feeny.Feeny;
import feeny.reader.ScopeStmt;

public class FunctionHeaderNode extends RootNode {
    final String args[];
    @Child RootNode body;
    final FrameSlot slots[];

    public FunctionHeaderNode(String[] args_, RootNode body_, FrameDescriptor frameDescriptor) {
        super(Feeny.class, null, frameDescriptor);
        args = args_;
        body = body_;
        slots = new FrameSlot[args.length];
        for (int i = 0; i < slots.length; ++i) {
            slots[i] = frameDescriptor.findOrAddFrameSlot(args[i]);
        }
    }

    @Override
    public Object execute(VirtualFrame frame) {
        for (int i = 0; i < slots.length; ++i) {
            frame.setObject(slots[i], frame.getArguments()[i]);
        }
        return body.execute(frame);
    }
}
