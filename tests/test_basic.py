import pytest

from testsuite.databases import pgsql


# Start the tests via `make test-debug` or `make test-release`

async def test_first_add_point(service_client):
    response = await service_client.get('/add', params={'x': 1, 'y': 1})
    assert response.status == 200
    assert response.text == "The point ({0}, {1}) has been created.\n".format(1, 1)


async def test_db_add_exist_point(service_client):
    response = await service_client.get('/add', params={'x': 2, 'y': 2})
    assert response.status == 200
    assert response.text == "The point ({0}, {1}) has been created.\n".format(2, 2)

    response = await service_client.get('/add', params={'x': 2, 'y': 2})
    assert response.status == 409
    assert response.text == "The point ({0}, {1}) already exists.\n".format(2, 2)

    response = await service_client.get('/add', params={'x': 2, 'y': 2})
    assert response.status == 409
    assert response.text == "The point ({0}, {1}) already exists.\n".format(2, 2)


async def test_db_bad_request_on_add_point(service_client):
    response = await service_client.get('/add', params={'x': 2})
    assert response.status == 400
    assert response.text == "No 'y' query argument.\n"

    response = await service_client.get('/add', params={'y': 2})
    assert response.status == 400
    assert response.text == "No 'x' query argument.\n"

    response = await service_client.get('/add', params={'x': 20.223, 'y': 2})
    assert response.status == 400
    assert response.text == "Invalid 'x' value.\n"

    response = await service_client.get('/add', params={'x': 2, 'y': 20.223})
    assert response.status == 400
    assert response.text == "Invalid 'y' value.\n"


async def test_request_result(service_client):
    response = await service_client.get('/result')
    assert response.status == 400
    assert response.text == "The polygon must have at least three vertices.\n"

    response = await service_client.get('/add', params={'x': 1, 'y': 0})
    assert response.status == 200
    assert response.text == "The point ({0}, {1}) has been created.\n".format(1, 0)

    response = await service_client.get('/add', params={'x': 1, 'y': 1})
    assert response.status == 200
    assert response.text == "The point ({0}, {1}) has been created.\n".format(1, 1)

    response = await service_client.get('/add', params={'x': 2, 'y': 1})
    assert response.status == 200
    assert response.text == "The point ({0}, {1}) has been created.\n".format(2, 1)

    response = await service_client.get('/add', params={'x': 2, 'y': 0})
    assert response.status == 200
    assert response.text == "The point ({0}, {1}) has been created.\n".format(2, 0)

    response = await service_client.get('/result')
    assert response.status == 200
    assert response.text == "Perimeter = {0}.\n".format(4)


@pytest.mark.pgsql('db_1', files=['initial_data.sql'])
async def test_db_initial_data(service_client):
    response = await service_client.get('/add', params={'x': 1, 'y': 1})
    assert response.status == 409
    assert response.text == "The point ({0}, {1}) already exists.\n".format(1, 1)
