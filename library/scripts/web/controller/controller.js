import { html, useState, useEffect, createContext, useRef, useContext } from './preact-standalone.js'

const set = (obj, path, value) => {
    if (!path || path == "") return value;

    const steps = path.split(".");
    const step = steps[0];
    const remainingSteps = steps.slice(1).join(".");

    if (Array.isArray(obj)) {
        const index = parseInt(step);
        const result = [...obj];
        result[index] = set(obj[index], remainingSteps, value);
        return result;
    } else if (typeof (obj) === "object") {
        const result = { ...obj };
        result[step] = set(obj[step], remainingSteps, value);
        return result;
    } else {
        console.error('unknown obj', obj, typeof (obj))
    }
}

const defaultColumn = {
    slots: [],
    name: "default column name"
}

const defaultSlot = {
    active:false,
    name: "default slot name"
}

const initialState = {
    columns: []
}

const SendMessage = createContext();

export const ControllerApp = (props) => {
    const [state, setState] = useState(initialState);
    const [socketState, setSocketState] = useState(WebSocket.CLOSED);
    const socketRef = useRef();

    useEffect(() => {

        const resizeController = (columnIndex, slotIndex) => 
        {
            setState(state => {
                if (columnIndex === undefined || state.columns.length > columnIndex)
                    return state;

                return set(state, `columns.${columnIndex}`, defaultColumn)
            })
            setState(state => {
                if (slotIndex === undefined || state.columns[columnIndex].slots.length > slotIndex)
                    return state;

                return set(state, `columns.${columnIndex}.slots.${slotIndex}`, defaultSlot)
            })
        }

        const createSocket = () => {
            const socket = new WebSocket(`wss://${location.host}:9800`);
            setSocketState(socket.readyState);

            socket.onmessage = wsmsg => {
                const msg = JSON.parse(wsmsg.data);

                resizeController(msg.columnIndex, msg.slotIndex)

                if (msg.type == "onHubSlotActiveChange") {
                    //console.log('onHubSlotActiveChange', msg)
                    setState(state => set(state, `columns.${msg.columnIndex}.slots.${msg.slotIndex}.active`, Boolean(msg.active)))
                }

                if (msg.type == "onHubSlotNameChange") {
                    //console.log('onHubSlotActiveChange', msg)
                    setState(state => set(state, `columns.${msg.columnIndex}.slots.${msg.slotIndex}.name`, msg.name))
                }
            }

            socket.onclose = (e) => {
                setSocketState(socket.readyState);
                console.log('Socket is closed. Reconnect will be attempted in 1 second.', e.reason);
                setTimeout(() => {
                    createSocket()
                }, 1000);
            };
            
            socket.onerror = (err) => {
                setSocketState(socket.readyState);
                console.error('Socket encountered error: ', err.message, 'Closing socket');
                socket.close();
            };

            socket.onopen = () => {
                setSocketState(socket.readyState);
            }

            socketRef.current = socket;

        }

        createSocket();
        return () => socketRef.current.close();
    }, [])

    const send = (msg) => {
        //console.log('sending', msg)
        socketRef.current.send(msg)
    }

    return html`
    <${SendMessage.Provider} value=${send}>
        <${SocketState} socketState=${socketState}/>
        <${Controller} state=${state} />
    </${SendMessage.Provider}>`;
}

const SocketState = ({socketState}) => {
    if (socketState==WebSocket.OPEN) return
    //const states = ["Connecting","Open","Closing","Closed"];
    //return html`<div class="controllerState">${states[socketState]}</div>`
    return html`<div class="controllerState">Reconnecting ...</div>`
}

const Controller = ({ state }) => {
    //console.log({state})
    return html`
    <div class="controller">
        ${state.columns.map((column, columnIndex) => html`<${Column} column=${column} columnIndex=${columnIndex}/>`)}
    </div>`;
}

const Column = ({ column, columnIndex }) => html`
    <div class="column">
        ${column.slots.map((slot,slotIndex) => html`<${Slot} slot=${slot} columnIndex=${columnIndex} slotIndex=${slotIndex}/>`)}
    </div>`;

const Slot = ({ slot, columnIndex, slotIndex }) => {
    const sender = useContext(SendMessage);

    const handlePressed  = () => {
        sender(`{"type":"buttonPressed", "columnIndex":${columnIndex}, "slotIndex": ${slotIndex}}`)
    }

    const handleReleased = () => {
        sender(`{"type":"buttonReleased", "columnIndex":${columnIndex}, "slotIndex": ${slotIndex}}`)
    }

    return html`
    <div 
        class="slot ${slot.active ? "active" : ""}" 
        onmousedown=${handlePressed}
        onmouseup=${handleReleased}
        ontouchstart=${(event)=>{handlePressed(); event.preventDefault();}}
        ontouchend=${(event)=>{handleReleased(); event.preventDefault();}}
        >
        ${slot.name}
    </div>`;
}