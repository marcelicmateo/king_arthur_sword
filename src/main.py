#!/bin/python3

from inspect import FullArgSpec
import dash
from dash.dependencies import Input, Output, State
import dash_core_components as dcc
import dash_html_components as html
from dataclasses import dataclass, field
from typing import List
from pathlib import Path
import ujson
from random import sample
from RpiMotorLib import rpiservolib


import time
import RPi.GPIO as GPIO

PIR_PIN = 6
IR_PIN = 26
SERVO_PIN = 13

GPIO.setmode(GPIO.BCM)
GPIO.setup(IR_PIN, GPIO.IN, pull_up_down=GPIO.PUD_UP)  # IF red sensor
GPIO.setup(PIR_PIN, GPIO.IN, pull_up_down=GPIO.PUD_DOWN)  # PIR sensor


def is_sword_locked():
    return False


def is_sword_present():
    if GPIO.input(IR_PIN) == 0:
        return True
    else:
        return False


def is_human_present():
    if GPIO.input(PIR_PIN) == 1:
        return True
    else:
        return False


@dataclass()
class system_settings:
    state: int = 0
    n_players: int = 0
    n_winners: int = 0
    w_numbers: List[int] = field(default_factory=list)
    current_player: int = 0
    random_chance: int = 0
    sword_locked: bool = field(init=False)
    sword_present: bool = field(init=False)
    human_state: bool = field(init=False)
    is_king: bool = False
    perma_lock: bool = False
    perma_unlock: bool = False
    STATE_PATH: str = "state.txt"
    CPLAYER_PATH: str = "cplayer.txt"
    RANDOM_PATH: str = "random.txt"
    BINGO_PATH: str = "bingo.txt"

    def _load_from_file(self, var, path):
        path = Path(path)
        if path.exists():
            with path.open("r") as f:
                var = f.read()
                return var
        else:
            path.open("w").write(str(var))
            return var

    def _load_bingo(self, path):
        bingo = {
            "n_players": self.n_players,
            "n_winners": self.n_winners,
            "w_numbers": self.w_numbers,
        }
        bingo = self._load_from_file(ujson.dumps(bingo), path)
        bingo = ujson.loads(bingo)
        self.n_winners = int(bingo.get("n_winners"))
        self.n_players = int(bingo.get("n_players"))
        self.w_numbers = bingo.get("w_numbers")
        return

    def __post_init__(self):
        self.w_numbers = [
            0,
        ]
        self.sword_locked = is_sword_locked()
        self.sword_present = is_sword_present()
        self.human_state = is_human_present()
        self.state = int(self._load_from_file(self.state, self.STATE_PATH))
        self.current_player = int(
            self._load_from_file(self.current_player, self.CPLAYER_PATH)
        )
        self.random_chance = int(
            self._load_from_file(self.random_chance, self.RANDOM_PATH)
        )
        self._load_bingo(self.BINGO_PATH)

    def _save_to_file(self, s, p):
        p = Path(p)
        p.open("w").write(s)
        return

    def change_state(self, new_state):
        self.state = new_state
        self._save_to_file(str(new_state), self.STATE_PATH)
        return

    def change_chance(self, new):
        self.random_chance = new
        self._save_to_file(str(new), self.RANDOM_PATH)
        return

    def change_cPlayer(self, new):
        self.current_player = new
        self._save_to_file(str(new), self.CPLAYER_PATH)
        return

    def generate_bingo(self, players, winners):
        if winners > players:
            winners = players
        self.n_players = players
        self.n_winners = winners
        self.change_cPlayer(0)
        self.w_numbers = sample(range(1, players + 1), winners)
        self.w_numbers.sort()
        bingo = {
            "n_players": self.n_players,
            "n_winners": self.n_winners,
            "w_numbers": self.w_numbers,
        }
        self._save_to_file(ujson.dumps(bingo), self.BINGO_PATH)


settings = system_settings()


def sword_returned():
    settings.sword_present = True
    return


def sword_removed():
    settings.sword_present = False
    return


def ir_interupt(pin):
    print("IR")
    print(GPIO.input(pin))
    if GPIO.input(pin) == 0:
        sword_returned()
    else:
        sword_removed()
    return


servo = rpiservolib.SG90servo("servoone", 50, 3, 11)

LOCK_POSITION = 11
UNLOCK_POSITION = 3


def lock_sword():
    servo.servo_move(SERVO_PIN, LOCK_POSITION, delay=0.1)
    settings.sword_locked = True
    return


def unlock_sword():
    servo.servo_move(SERVO_PIN, UNLOCK_POSITION, 0.1)
    settings.sword_locked = False
    return


from random import randint


def human_on_platform():
    settings.human_state = True
    if settings.perma_lock or settings.perma_unlock:
        return
    elif settings.is_king == True:
        return
    elif settings.state == 2:
        # play bingo
        settings.change_cPlayer(settings.current_player + 1)
        if settings.current_player in settings.w_numbers:
            # release sword
            settings.is_king = True
            unlock_sword()
        return
    elif settings.state == 3:
        # play random king
        if randint(1, 100) < settings.random_chance:
            settings.is_king = True
            unlock_sword()
        return
    return


def human_left_platform():
    settings.human_state = False
    if settings.perma_lock or settings.perma_unlock:
        return
    if settings.is_king == True and settings.sword_present == True:
        settings.is_king == False
        lock_sword()
        return
    return


def pir_interupt(pin):
    print("Pir int")
    print(GPIO.input(pin))
    if GPIO.input(pin) == 0:
        human_left_platform()
    else:
        human_on_platform()
    return


GPIO.add_event_detect(IR_PIN, GPIO.BOTH, callback=ir_interupt, bouncetime=200)
GPIO.add_event_detect(PIR_PIN, GPIO.BOTH, callback=pir_interupt, bouncetime=200)


app = dash.Dash(__name__, title="Kralj Arthur")


app.layout = html.Div(
    children=[
        html.Div(
            className="topnav",
            children=[
                html.H1(children="Kontrola"),
                html.Div(
                    children=[
                        html.P(
                            id="error_status",
                            className="error_status",
                            children=[
                                html.Span(id="error_msg"),
                                html.Button("Reset", id="err_button", n_clicks=0),
                            ],
                        )
                    ],
                ),
            ],
        ),
        html.Div(
            className="content",
            children=html.Div(
                className="card",
                children=[
                    html.H2("Način rada sustava"),
                    # html.P(
                    #    className="state",
                    #    id="status_nacin_rada",
                    #    children="$nacinRADA",
                    # ),
                    dcc.Dropdown(
                        searchable=False,
                        id="new_state",
                        className="select",
                        persistence=True,
                        value=settings.state,
                        options=[
                            {
                                "label": "1 - Mac upotpunosti OTKLJUCAN",
                                "value": 0,
                            },
                            {
                                "label": "2 - Mac upotpunosti ZAKLJUCAN",
                                "value": 1,
                            },
                            {"label": "3 - Play BINGO", "value": 2},
                            {"label": "4 - RANDOM KING", "value": 3},
                        ],
                    ),
                ],
            ),
        ),
        html.Div(
            className="content",
            id="div_bingo",
            children=html.Div(
                className="card",
                children=[
                    html.H2("Play BINGO"),
                    html.P(
                        className="state",
                        children=[
                            html.Span("Broj Igraca: "),
                            html.Span("$nacinRADA", id="num_players"),
                        ],
                    ),
                    html.P(
                        className="state",
                        children=[
                            html.Span("Broj dobitnika: "),
                            html.Span("$nacinRADA", id="num_winers"),
                        ],
                    ),
                    html.P(
                        className="state",
                        children=[
                            html.Span("Dobitni brojevi: "),
                            html.Span("$nacinRADA", id="win_numbers"),
                        ],
                    ),
                    html.P(
                        className="state",
                        children=[
                            html.Span("Trenutni igrac: "),
                            html.Span("$nacinRADA", id="current_player"),
                        ],
                    ),
                    html.P(
                        className="state",
                        children=[
                            "Broj Igrača",
                            dcc.Input(
                                id="new_number_of_players",
                                type="number",
                                min=0,
                                max=1000,
                                debounce=True,
                                required=True,
                            ),
                        ],
                    ),
                    html.P(
                        className="state",
                        children=[
                            "Broj Dobitnika",
                            dcc.Input(
                                id="new_number_of_winners",
                                type="number",
                                min=0,
                                max=1000,
                                debounce=True,
                                required=True,
                            ),
                        ],
                    ),
                    html.Button("Generiraj novi bingo", id="submit_button", n_clicks=0),
                ],
            ),
        ),
        html.Div(
            className="content",
            id="div_random",
            children=html.Div(
                className="card",
                children=[
                    html.H2("Random KING chance"),
                    html.P(
                        className="state",
                        children=[
                            html.Span("Trenutna sansa"),
                            html.Span(settings.random_chance, id="current_probability"),
                        ],
                    ),
                    html.P(
                        className="state",
                        children=[
                            html.Span("Nova sansa "),
                            dcc.Input(
                                type="number",
                                max=100,
                                min=0,
                                id="new_probability",
                                debounce=True,
                            ),
                        ],
                    ),
                ],
            ),
        ),
    ]
)

import threading


def prepare_bingo_chance(state):
    # prepare for bingo, lock sword
    if settings.sword_present == True and settings.sword_locked == True:
        return
    elif settings.sword_present == True and settings.sword_locked == False:
        lock_sword()
        return
    elif settings.sword_present == False:
        while settings.sword_present == False and settings.state == state:
            time.sleep(1)
        if settings.state == state:
            lock_sword()
        return


def lock_state(s):
    # lock sword state, wait for sword to return
    if settings.sword_present == True:
        lock_sword()
    else:
        while (settings.sword_present == False) and settings.state == s:
            time.sleep(1)
        if settings.state == s:
            lock_sword()
    return


@app.callback(
    [
        # Output(component_id="status_nacin_rada", component_property="children"),
        Output(component_id="div_bingo", component_property="hidden"),
        Output(component_id="div_random", component_property="hidden"),
    ],
    Input(component_id="new_state", component_property="value"),
)
def change_state(new_value: int):
    # v = [
    #    "1 - Mac upotpunosti OTKLJUCAN",
    #    "2 - Mac upotpunosti ZAKLJUCAN",
    #    "3 - Play BINGO",
    #    "4 - RANDOM KING",
    # ]
    settings.change_state(new_value)
    if new_value == 0:
        settings.perma_unlock = True
        unlock_sword()
    elif new_value == 1:
        settings.perma_lock = True
        y = threading.Thread(target=lock_state, args=(new_value,))
        y.start()
    else:
        settings.perma_unlock = False
        settings.perma_lock = False
        x = threading.Thread(target=prepare_bingo_chance, args=(new_value,))
        x.start()
    return (
        # v[(new_value)],
        False if new_value == 2 else True,
        False if new_value == 3 else True,
    )


@app.callback(
    Output("current_probability", "children"), Input("new_probability", "value")
)
def change_random_chance(new):
    if new is None:
        new = settings.random_chance
        return new
    else:
        settings.change_chance(new)
        return new


@app.callback(
    [
        Output("current_player", "children"),
        Output("win_numbers", "children"),
        Output("num_winers", "children"),
        Output("num_players", "children"),
    ],
    Input("submit_button", "n_clicks"),
    [State("new_number_of_players", "value"), State("new_number_of_winners", "value")],
)
def generate_new_bingo(n, new_n_players, new_n_winners):
    if n == 0 or new_n_winners is None or new_n_players is None:
        return (
            settings.current_player,
            ",".join(map(str, settings.w_numbers)),
            settings.n_winners,
            settings.n_players,
        )
    else:
        settings.generate_bingo(new_n_players, new_n_winners)
        return (
            settings.current_player,
            ",".join(map(str, settings.w_numbers)),
            settings.n_winners,
            settings.n_players,
        )


if __name__ == "__main__":
    app.run_server(debug=True, dev_tools_hot_reload=True)
